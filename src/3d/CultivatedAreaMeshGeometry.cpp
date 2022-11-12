// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "CultivatedAreaMeshGeometry.h"
#include <QVector3D>
#include <QVector>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>
#include <utility>

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

#include "job/CultivatedAreaOptimitionController.h"

CultivatedAreaMeshGeometry::CultivatedAreaMeshGeometry( Qt3DCore::QNode* parent )
    : Qt3DCore::QGeometry( parent )
    , m_positionAttribute( new Qt3DCore::QAttribute( this ) )
    , m_normalAttribute( new Qt3DCore::QAttribute( this ) )
    , m_tangentAttribute( new Qt3DCore::QAttribute( this ) )
    , m_indexAttribute( new Qt3DCore::QAttribute( this ) )
    , m_vertexBuffer( new Qt3DCore::QBuffer( this ) )
    , m_indexBuffer( new Qt3DCore::QBuffer( this ) ) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos, vec3 normal, vec4 tangent
  constexpr uint32_t elementSize = 3 + 3 + 4;
  constexpr uint32_t stride      = elementSize * sizeof( float );

  m_positionAttribute->setName( Qt3DCore::QAttribute::defaultPositionAttributeName() );
  m_positionAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
  m_positionAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );
  m_positionAttribute->setByteStride( stride );
  m_positionAttribute->setCount( 0 );

  m_normalAttribute->setName( Qt3DCore::QAttribute::defaultNormalAttributeName() );
  m_normalAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_normalAttribute->setVertexSize( 3 );
  m_normalAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_normalAttribute->setBuffer( m_vertexBuffer );
  m_normalAttribute->setByteStride( stride );
  m_normalAttribute->setByteOffset( 3 * sizeof( float ) );
  m_normalAttribute->setCount( 0 );

  m_tangentAttribute->setName( Qt3DCore::QAttribute::defaultTangentAttributeName() );
  m_tangentAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_tangentAttribute->setVertexSize( 4 );
  m_tangentAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_tangentAttribute->setBuffer( m_vertexBuffer );
  m_tangentAttribute->setByteStride( stride );
  m_tangentAttribute->setByteOffset( 6 * sizeof( float ) );
  m_tangentAttribute->setCount( 0 );

  m_indexAttribute->setAttributeType( Qt3DCore::QAttribute::IndexAttribute );
  m_indexAttribute->setVertexBaseType( Qt3DCore::QAttribute::UnsignedShort );
  m_indexAttribute->setBuffer( m_indexBuffer );
  m_indexAttribute->setCount( 0 );

  addAttribute( m_positionAttribute );
  addAttribute( m_normalAttribute );
  addAttribute( m_tangentAttribute );
  addAttribute( m_indexAttribute );

  trackPointsLeft   = std::make_shared< std::vector< Point_3 > >();
  trackPointsRight  = std::make_shared< std::vector< Point_3 > >();
  trackNormalsLeft  = std::make_shared< std::vector< QVector3D > >();
  trackNormalsRight = std::make_shared< std::vector< QVector3D > >();

  trackPointsLeft->reserve( 300 );
  trackPointsRight->reserve( 300 );
  trackNormalsLeft->reserve( 300 );
  trackNormalsRight->reserve( 300 );
}

CultivatedAreaMeshGeometry::~CultivatedAreaMeshGeometry() = default;

void
CultivatedAreaMeshGeometry::addTrackMeshGeometry( CultivatedAreaMeshGeometry* trackMeshGeometry ) {
  {
    size_t size = trackMeshGeometry->trackPointsLeft->size();

    if( size > 2 ) {
      trackPointsLeft->reserve( size );
      std::copy(
        trackMeshGeometry->trackPointsLeft->cbegin(), trackMeshGeometry->trackPointsLeft->cend(), std::back_inserter( *trackPointsLeft ) );
    }
  }
  {
    size_t size = trackMeshGeometry->trackPointsRight->size();

    if( size > 2 ) {
      trackPointsRight->reserve( size );
      std::copy( trackMeshGeometry->trackPointsRight->cbegin(),
                 trackMeshGeometry->trackPointsRight->cend(),
                 std::back_inserter( *trackPointsRight ) );
    }
  }
}

void
CultivatedAreaMeshGeometry::updateBuffers() {
  if( trackPointsLeft->size() > 1 && trackPointsRight->size() > 1 ) {
    const int nVerts = trackPointsLeft->size() + trackPointsRight->size();

    QByteArray bufferBytes;
    QByteArray indexBytes;

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec3 normal, vec4 tangent
    constexpr int elementSizeVertices = 3 + 3 + 4;
    constexpr int strideVertices      = elementSizeVertices * sizeof( float );
    bufferBytes.resize( strideVertices * nVerts );

    const int triangles = nVerts - 2;
    const int indices   = 3 * triangles;
    Q_ASSERT( indices < std::numeric_limits< uint16_t >::max() );
    indexBytes.resize( indices * sizeof( uint16_t ) );

    auto  iteratorLeftPoints   = trackPointsLeft->cbegin();
    auto  iteratorLeftNormals  = trackNormalsLeft->cbegin();
    auto  iteratorRightPoints  = trackPointsRight->cbegin();
    auto  iteratorRightNormals = trackNormalsRight->cbegin();
    auto* fptr                 = reinterpret_cast< float* >( bufferBytes.data() );
    auto* indexPtr             = reinterpret_cast< uint16_t* >( indexBytes.data() );

    // left point
    {
      // position
      *fptr++ = float( iteratorLeftPoints->x() );
      *fptr++ = float( iteratorLeftPoints->y() );
      *fptr++ = float( iteratorLeftPoints->z() );
      // normal
      *fptr++ = float( iteratorLeftNormals->x() );
      *fptr++ = float( iteratorLeftNormals->y() );
      *fptr++ = float( iteratorLeftNormals->z() );
      // tangent
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
    }

    // right point
    {
      // position
      *fptr++ = float( iteratorRightPoints->x() );
      *fptr++ = float( iteratorRightPoints->y() );
      *fptr++ = float( iteratorRightPoints->z() );
      // normal
      *fptr++ = float( iteratorRightNormals->x() );
      *fptr++ = float( iteratorRightNormals->y() );
      *fptr++ = float( iteratorRightNormals->z() );
      // tangent
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
    }

    ++iteratorLeftPoints;
    ++iteratorRightPoints;

    uint16_t counter          = 1;
    uint16_t counterLastLeft  = 0;
    uint16_t counterLastRight = 1;

    while( true ) {
      // left point
      if( iteratorLeftPoints != trackPointsLeft->cend() ) {
        // position
        *fptr++ = float( iteratorLeftPoints->x() );
        *fptr++ = float( iteratorLeftPoints->y() );
        *fptr++ = float( iteratorLeftPoints->z() );
        // normal
        *fptr++ = float( iteratorLeftNormals->x() );
        *fptr++ = float( iteratorLeftNormals->y() );
        *fptr++ = float( iteratorLeftNormals->z() );
        // tangent
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;

        ++counter;

        *indexPtr++ = counter;
        *indexPtr++ = counterLastLeft;
        *indexPtr++ = counterLastRight;

        counterLastLeft = counter;
        ++iteratorLeftPoints;
        ++iteratorLeftNormals;
      }

      // right point
      if( iteratorRightPoints != trackPointsRight->cend() ) {
        // position
        *fptr++ = float( iteratorRightPoints->x() );
        *fptr++ = float( iteratorRightPoints->y() );
        *fptr++ = float( iteratorRightPoints->z() );
        // normal
        *fptr++ = float( iteratorRightNormals->x() );
        *fptr++ = float( iteratorRightNormals->y() );
        *fptr++ = float( iteratorRightNormals->z() );
        // tangent
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;

        ++counter;

        *indexPtr++ = counter;
        *indexPtr++ = counterLastLeft;
        *indexPtr++ = counterLastRight;

        counterLastRight = counter;
        ++iteratorRightPoints;
        ++iteratorRightNormals;
      }

      if( ( iteratorLeftPoints == trackPointsLeft->cend() ) && ( iteratorRightPoints == trackPointsRight->cend() ) ) {
        break;
      }
    }

    m_vertexBuffer->setData( bufferBytes );
    m_positionAttribute->setCount( nVerts );
    m_normalAttribute->setCount( nVerts );
    m_tangentAttribute->setCount( nVerts );

    m_indexBuffer->setData( indexBytes );
    m_indexAttribute->setCount( indices );

    qDebug() << "CultivatedAreaMeshGeometry::updateBuffers()" << indices << nVerts << trackPointsLeft->size() << trackPointsRight->size();
  } else {
    m_positionAttribute->setCount( 0 );
    m_normalAttribute->setCount( 0 );
    m_tangentAttribute->setCount( 0 );
    m_indexAttribute->setCount( 0 );
    qDebug() << "CultivatedAreaMeshGeometry::updateBuffers()" << 0;
  }
}

void
CultivatedAreaMeshGeometry::optimise() {
  // deletes the old one automatically and therefore stops all the old jobs
  if( cultivatedAreaOptimitionControllerLeft ) {
    cultivatedAreaOptimitionControllerLeft->stop();
  }

  if( cultivatedAreaOptimitionControllerRight ) {
    cultivatedAreaOptimitionControllerRight->stop();
  }

  //    fieldOptimitionController.clear();
  cultivatedAreaOptimitionControllerLeft  = new CultivatedAreaOptimitionController( trackPointsLeft, trackNormalsLeft );
  cultivatedAreaOptimitionControllerRight = new CultivatedAreaOptimitionController( trackPointsRight, trackNormalsRight );

  QObject::connect( cultivatedAreaOptimitionControllerLeft,
                    &CultivatedAreaOptimitionController::simplifyPolylineResult,
                    this,
                    &CultivatedAreaMeshGeometry::simplifyPolylineResultLeft );
  QObject::connect( cultivatedAreaOptimitionControllerRight,
                    &CultivatedAreaOptimitionController::simplifyPolylineResult,
                    this,
                    &CultivatedAreaMeshGeometry::simplifyPolylineResultRight );

  cultivatedAreaOptimitionControllerLeft->run( maxDeviation );
  cultivatedAreaOptimitionControllerRight->run( maxDeviation );

  //  if( trackPointsLeft->size() > 2 ) {
  //    auto* cgalWorkerLeft = new CgalWorker();
  //    cgalWorkerLeft->moveToThread( thread );

  //    QObject::connect( this, &CultivatedAreaMeshGeometry::simplifyPolylineLeft,
  //    cgalWorkerLeft, &CgalWorker::simplifyPolyline_3 ); QObject::connect(
  //    cgalWorkerLeft, &CgalWorker::simplifyPolylineResult_3, this,
  //    &CultivatedAreaMeshGeometry::simplifyPolylineResultLeft ); waitForOptimition =
  //    !waitForOptimition;

  //    Q_EMIT simplifyPolylineLeft( &trackPointsLeft, maxDeviation );
  //  }

  //  if( trackPointsRight->size() > 2 ) {
  //    auto* cgalWorkerRight = new CgalWorker();
  //    cgalWorkerRight->moveToThread( thread );

  //    QObject::connect( this, &CultivatedAreaMeshGeometry::simplifyPolylineRight,
  //    cgalWorkerRight, &CgalWorker::simplifyPolyline_3 ); QObject::connect(
  //    cgalWorkerRight, &CgalWorker::simplifyPolylineResult_3, this,
  //    &CultivatedAreaMeshGeometry::simplifyPolylineResultRight ); waitForOptimition =
  //    !waitForOptimition;

  //    Q_EMIT simplifyPolylineRight( &trackPointsRight, maxDeviation );
  //  }

  qDebug() << "CultivatedAreaMeshGeometry::optimise" << trackPointsLeft->size() << trackPointsRight->size();
}

void
CultivatedAreaMeshGeometry::simplifyPolylineResultLeft( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                                                        std::shared_ptr< std::vector< QVector3D > > resultNormals,
                                                        const double                                maxDeviation ) {
  std::cout << "CultivatedAreaMeshGeometry::simplifyPolylineResultLeft " << trackPointsLeft->size() << " -> " << resultPoints->size()
            << std::endl;

  trackPointsLeft  = resultPoints;
  trackNormalsLeft = resultNormals;

  cultivatedAreaOptimitionControllerLeft->deleteLater();

  waitForOptimition = !waitForOptimition;

  if( !waitForOptimition ) {
    updateBuffers();
  }

  //  std::unique_ptr<std::vector<Point_3>> pointsPtr( points );

  //  if( pointsPtr->size() != trackPointsLeft->size() ) {
  //    auto sizeBefore = trackPointsLeft->size();

  //    double minDistance = std::numeric_limits<double>::infinity();

  //    if( trackPointsLeft->size() > 1 ) {
  //      for( auto p1 = trackPointsLeft->cbegin(), p2 = trackPointsLeft->cbegin() + 1,
  //      end = trackPointsLeft->cend();
  //           p2 != end;
  //           ++p1, ++p2 ) {
  ////        std::cout << "p1: " << *p1 << ", p2: " << *p2 << ", distance: " <<
  /// CGAL::squared_distance( *p1, *p2 ) << ", minDistance: " << minDistance << std::endl;
  //        minDistance = std::min( minDistance, CGAL::squared_distance( *p1, *p2 ) );
  //      }
  //    }

  //    // correct scaling: /4 on the squared value equals /2 on the not squared
  //    minDistance /= 4;

  //    auto trackPointsBuffer( trackPointsLeft );
  //    auto trackNormalsBuffer( trackNormalsLeft );

  //    trackPointsLeft->clear();
  //    trackPointsLeft->reserve( pointsPtr->size() );
  //    trackNormalsLeft->clear();
  //    trackNormalsLeft->reserve( pointsPtr->size() );

  //    {
  //      auto n1 = trackNormalsBuffer.cbegin();
  //      auto p2 = pointsPtr->cbegin();

  //      int countP1 = 0;
  //      int countP2 = 0;

  //      for( auto p1 = trackPointsBuffer.cbegin(), endP1 = trackPointsBuffer.cend();
  //           p1 != endP1;
  //           ++p1 ) {
  ////        std::cout << "p1: " << *p1 << ", p2: " << *p2 << ", distance: " <<
  /// CGAL::squared_distance( *p1, *p2 ) << ", minDistance: " << minDistance << std::endl;

  //        if( CGAL::squared_distance( *p1, *p2 ) < minDistance ) {
  //          trackPointsLeft->push_back( *p1 );
  //          trackNormalsLeft->push_back( *n1 );
  //          ++countP2;
  ////          std::cout << "added left" << *p2 << " (" << countP1 << ", " << countP2 <<
  ///", " << pointsPtr->size() << ")" << std::endl;
  //          ++p2;
  //        }

  //        ++n1;
  //        ++countP1;
  //      }
  //    }

  //    qDebug() << "simplifyPolylineResultLeft" << trackPointsBuffer.size() <<
  //    trackPointsLeft->size() << trackNormalsLeft->size() << pointsPtr->size();

  //    sender()->deleteLater();

  //    waitForOptimition = !waitForOptimition;

  //    if( !waitForOptimition ) {
  //      updateBuffers();
  //    }
  //  }
}

void
CultivatedAreaMeshGeometry::simplifyPolylineResultRight( std::shared_ptr< std::vector< Point_3 > >   resultPoints,
                                                         std::shared_ptr< std::vector< QVector3D > > resultNormals,
                                                         const double                                maxDeviation ) {
  std::cout << "CultivatedAreaMeshGeometry::simplifyPolylineResultLeft " << trackPointsRight->size() << " -> " << resultPoints->size()
            << std::endl;

  trackPointsRight  = resultPoints;
  trackNormalsRight = resultNormals;

  cultivatedAreaOptimitionControllerRight->deleteLater();

  waitForOptimition = !waitForOptimition;

  if( !waitForOptimition ) {
    updateBuffers();
  }

  //  std::unique_ptr<std::vector<Point_3>> pointsPtr( points );

  //  if( pointsPtr->size() != trackPointsRight->size() ) {
  //    auto sizeBefore = trackPointsRight->size();

  //    double minDistance = std::numeric_limits<double>::infinity();

  //    if( trackPointsRight->size() > 1 ) {
  //      for( auto p1 = trackPointsRight->cbegin(), p2 = trackPointsRight->cbegin() + 1,
  //      end = trackPointsRight->cend();
  //           p2 != end;
  //           ++p1, ++p2 ) {
  ////        std::cout << "p1: " << *p1 << ", p2: " << *p2 << ", distance: " <<
  /// CGAL::squared_distance( *p1, *p2 ) << ", minDistance: " << minDistance << std::endl;
  //        minDistance = std::min( minDistance, CGAL::squared_distance( *p1, *p2 ) );
  //      }
  //    }

  //    // correct scaling: /4 on the squared value equals /2 on the not squared
  //    minDistance /= 4;

  //    auto trackPointsBuffer( trackPointsRight );
  //    auto trackNormalsBuffer( trackNormalsRight );

  //    trackPointsRight->clear();
  //    trackPointsRight->reserve( pointsPtr->size() );
  //    trackNormalsRight->clear();
  //    trackNormalsRight->reserve( pointsPtr->size() );

  //    {
  //      auto n1 = trackNormalsBuffer.cbegin();
  //      auto p2 = pointsPtr->cbegin();

  //      int countP1 = 0;
  //      int countP2 = 0;

  //      for( auto p1 = trackPointsBuffer.cbegin(), endP1 = trackPointsBuffer.cend();
  //           p1 != endP1;
  //           ++p1 ) {
  ////        std::cout << "p1: " << *p1 << ", p2: " << *p2 << ", distance: " <<
  /// CGAL::squared_distance( *p1, *p2 ) << ", minDistance: " << minDistance << std::endl;

  //        if( CGAL::squared_distance( *p1, *p2 ) < minDistance ) {
  //          trackPointsRight->push_back( *p1 );
  //          trackNormalsRight->push_back( *n1 );
  //          ++countP2;
  ////          std::cout << "added right" << *p2 << " (" << countP1 << ", " << countP2 <<
  ///", " << pointsPtr->size() << ")" << std::endl;
  //          ++p2;
  //        }

  //        ++n1;
  //        ++countP1;
  //      }
  //    }

  //    qDebug() << "simplifyPolylineResultLeft" << trackPointsBuffer.size() <<
  //    trackPointsRight->size() << trackNormalsRight->size() << pointsPtr->size();

  //    sender()->deleteLater();

  //    waitForOptimition = !waitForOptimition;

  //    if( !waitForOptimition ) {
  //      updateBuffers();
  //    }
  //  }
}

int
CultivatedAreaMeshGeometry::vertexCount() {
  return m_indexAttribute->count();
}

void
CultivatedAreaMeshGeometry::addPoints( const Point_3 pointLeft, const Point_3 pointRight, const QVector3D normalVector ) {
  addPointLeftWithoutUpdate( pointLeft, normalVector );
  addPointRightWithoutUpdate( pointRight, normalVector );

  updateBuffers();
}

void
CultivatedAreaMeshGeometry::addPointLeft( const Point_3 point, const QVector3D normalVector ) {
  addPointLeftWithoutUpdate( point, normalVector );

  updateBuffers();
}

void
CultivatedAreaMeshGeometry::addPointRight( const Point_3 point, const QVector3D normalVector ) {
  addPointRightWithoutUpdate( point, normalVector );

  updateBuffers();
}

void
CultivatedAreaMeshGeometry::clear() {
  qDebug() << "CultivatedAreaMeshGeometry::clear()";

  trackPointsLeft->clear();
  trackNormalsLeft->clear();
  trackPointsRight->clear();
  trackNormalsRight->clear();
  updateBuffers();
}

void
CultivatedAreaMeshGeometry::addPointLeftWithoutUpdate( const Point_3 point, const QVector3D normalVector ) {
  if( !trackPointsLeft->empty() ) {
    if( CGAL::squared_distance( point, trackPointsLeft->back() ) > 0.001 ) {
      trackPointsLeft->push_back( point );
      trackNormalsLeft->push_back( normalVector );
    }
  } else {
    trackPointsLeft->push_back( point );
    trackNormalsRight->push_back( normalVector );
  }
}

void
CultivatedAreaMeshGeometry::addPointRightWithoutUpdate( const Point_3 point, const QVector3D normalVector ) {
  if( !trackPointsRight->empty() ) {
    if( CGAL::squared_distance( point, trackPointsRight->back() ) > 0.001 ) {
      trackPointsRight->push_back( point );
      trackNormalsRight->push_back( normalVector );
    }
  } else {
    trackPointsRight->push_back( point );
    trackNormalsRight->push_back( normalVector );
  }
}

#include "moc_CultivatedAreaMeshGeometry.cpp"
