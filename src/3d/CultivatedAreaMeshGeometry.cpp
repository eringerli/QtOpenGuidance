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
#include <QVector>
#include <QVector3D>

#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

CultivatedAreaMeshGeometry::CultivatedAreaMeshGeometry( Qt3DCore::QNode* parent )
  : Qt3DRender::QGeometry( parent ),
    m_positionAttribute( new Qt3DRender::QAttribute( this ) ),
    m_normalAttribute( new Qt3DRender::QAttribute( this ) ),
    m_tangentAttribute( new Qt3DRender::QAttribute( this ) ),
    m_indexAttribute( new Qt3DRender::QAttribute( this ) ),
    m_vertexBuffer( new Qt3DRender::QBuffer( this ) ),
    m_indexBuffer( new Qt3DRender::QBuffer( this ) ) {

  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos, vec3 normal, vec4 tangent
  constexpr uint32_t elementSize = 3 + 3 + 4;
  constexpr uint32_t stride = elementSize * sizeof( float );

  m_positionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );
  m_positionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
  m_positionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );
  m_positionAttribute->setByteStride( stride );
  m_positionAttribute->setCount( 0 );

  m_normalAttribute->setName( Qt3DRender::QAttribute::defaultNormalAttributeName() );
  m_normalAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_normalAttribute->setVertexSize( 3 );
  m_normalAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_normalAttribute->setBuffer( m_vertexBuffer );
  m_normalAttribute->setByteStride( stride );
  m_normalAttribute->setByteOffset( 3 * sizeof( float ) );
  m_normalAttribute->setCount( 0 );

  m_tangentAttribute->setName( Qt3DRender::QAttribute::defaultTangentAttributeName() );
  m_tangentAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_tangentAttribute->setVertexSize( 4 );
  m_tangentAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_tangentAttribute->setBuffer( m_vertexBuffer );
  m_tangentAttribute->setByteStride( stride );
  m_tangentAttribute->setByteOffset( 6 * sizeof( float ) );
  m_tangentAttribute->setCount( 0 );

  m_indexAttribute->setAttributeType( Qt3DRender::QAttribute::IndexAttribute );
  m_indexAttribute->setVertexBaseType( Qt3DRender::QAttribute::UnsignedShort );
  m_indexAttribute->setBuffer( m_indexBuffer );
  m_indexAttribute->setCount( 0 );

  addAttribute( m_positionAttribute );
  addAttribute( m_normalAttribute );
  addAttribute( m_tangentAttribute );
  addAttribute( m_indexAttribute );

  trackPointsLeft.reserve( 300 );
  trackPointsRight.reserve( 300 );

}

CultivatedAreaMeshGeometry::~CultivatedAreaMeshGeometry() = default;

void CultivatedAreaMeshGeometry::addTrackMeshGeometry( CultivatedAreaMeshGeometry* trackMeshGeometry ) {
  {
    size_t size = trackMeshGeometry->trackPointsLeft.size();

    if( size > 2 ) {
      trackPointsLeft.reserve( size );
      std::copy( trackMeshGeometry->trackPointsLeft.cbegin(), trackMeshGeometry->trackPointsLeft.cend(), std::back_inserter( trackPointsLeft ) );
    }
  }
  {
    size_t size = trackMeshGeometry->trackPointsRight.size();

    if( size > 2 ) {
      trackPointsRight.reserve( size );
      std::copy( trackMeshGeometry->trackPointsRight.cbegin(), trackMeshGeometry->trackPointsRight.cend(), std::back_inserter( trackPointsRight ) );
    }
  }
}

void CultivatedAreaMeshGeometry::updateBuffers() {
  if( trackPointsLeft.size() > 1 && trackPointsRight.size() > 1 ) {
    const int nVerts = trackPointsLeft.size() + trackPointsRight.size();

    QByteArray bufferBytes;
    QByteArray indexBytes;

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec3 normal, vec4 tangent
    constexpr int elementSizeVertices = 3 + 3 + 4;
    constexpr int strideVertices = elementSizeVertices * sizeof( float );
    bufferBytes.resize( strideVertices * nVerts );

    const int triangles = nVerts - 2;
    const int indices = 3 * triangles;
    Q_ASSERT( indices < std::numeric_limits<uint16_t>::max() );
    indexBytes.resize( indices * sizeof( uint16_t ) );

    auto iteratorLeftPoints = trackPointsLeft.cbegin();
    auto iteratorRightPoints = trackPointsRight.cbegin();
    auto* fptr = reinterpret_cast<float*>( bufferBytes.data() );
    auto* indexPtr = reinterpret_cast<uint16_t*>( indexBytes.data() );

    // left point
    {
      // position
      *fptr++ = float( iteratorLeftPoints->x() );
      *fptr++ = float( iteratorLeftPoints->y() );
      *fptr++ = 0.0f;
      // normal
      *fptr++ = 0.0f;
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
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
      *fptr++ = 0.0f;
      // normal
      *fptr++ = 0.0f;
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
      // tangent
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
      *fptr++ = 0.0f;
      *fptr++ = 1.0f;
    }

    ++iteratorLeftPoints;
    ++iteratorRightPoints;

    uint16_t counter = 1;
    uint16_t counterLastLeft = 0;
    uint16_t counterLastRight = 1;

    while( true ) {
      // left point
      if( iteratorLeftPoints != trackPointsLeft.cend() ) {
        // position
        *fptr++ = float( iteratorLeftPoints->x() );
        *fptr++ = float( iteratorLeftPoints->y() );
        *fptr++ = 0.0f;
        // normal
        *fptr++ = 0.0f;
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;
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
      }

      // right point
      if( iteratorRightPoints != trackPointsRight.cend() ) {
        // position
        *fptr++ = float( iteratorRightPoints->x() );
        *fptr++ = float( iteratorRightPoints->y() );
        *fptr++ = 0.0f;
        // normal
        *fptr++ = 0.0f;
        *fptr++ = 0.0f;
        *fptr++ = 1.0f;
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
      }

      if( ( iteratorLeftPoints == trackPointsLeft.cend() ) && ( iteratorRightPoints == trackPointsRight.cend() ) ) {
        break;
      }
    }

    m_vertexBuffer->setData( bufferBytes );
    m_positionAttribute->setCount( nVerts );
    m_normalAttribute->setCount( nVerts );
    m_tangentAttribute->setCount( nVerts );

    m_indexBuffer->setData( indexBytes );
    m_indexAttribute->setCount( indices );

    Q_EMIT vertexCountChanged( indices );
  } else {
    Q_EMIT vertexCountChanged( 0 );
  }
}

void CultivatedAreaMeshGeometry::optimise( CgalThread* thread ) {
  if( trackPointsLeft.size() > 2 ) {
    auto* cgalWorkerLeft = new CgalWorker();
    cgalWorkerLeft->moveToThread( thread );

    QObject::connect( this, &CultivatedAreaMeshGeometry::simplifyPolylineLeft, cgalWorkerLeft, &CgalWorker::simplifyPolyline );
    QObject::connect( cgalWorkerLeft, &CgalWorker::simplifyPolylineResult, this, &CultivatedAreaMeshGeometry::simplifyPolylineResultLeft );
    waitForOptimition = !waitForOptimition;

    Q_EMIT simplifyPolylineLeft( &trackPointsLeft, maxDeviation );
  }

  if( trackPointsRight.size() > 2 ) {
    auto* cgalWorkerRight = new CgalWorker();
    cgalWorkerRight->moveToThread( thread );

    QObject::connect( this, &CultivatedAreaMeshGeometry::simplifyPolylineRight, cgalWorkerRight, &CgalWorker::simplifyPolyline );
    QObject::connect( cgalWorkerRight, &CgalWorker::simplifyPolylineResult, this, &CultivatedAreaMeshGeometry::simplifyPolylineResultRight );
    waitForOptimition = !waitForOptimition;

    Q_EMIT simplifyPolylineRight( &trackPointsRight, maxDeviation );
  }
}

void CultivatedAreaMeshGeometry::simplifyPolylineResultLeft( std::vector<Point_2>* points ) {
  std::unique_ptr<std::vector<Point_2>> pointsPtr( points );

  auto sizeBefore = trackPointsLeft.size();

  trackPointsLeft.clear();
  trackPointsLeft.reserve( pointsPtr->size() );

  std::copy( pointsPtr->cbegin(), pointsPtr->cend(), std::back_inserter( trackPointsLeft ) );

  qDebug() << "simplifyPolylineResultLeft" << sizeBefore << trackPointsLeft.size();;

  sender()->deleteLater();

  waitForOptimition = !waitForOptimition;

  if( !waitForOptimition ) {
    updateBuffers();
  }
}

void CultivatedAreaMeshGeometry::simplifyPolylineResultRight( std::vector<Point_2>* points ) {
  std::unique_ptr<std::vector<Point_2>> pointsPtr( points );

  auto sizeBefore = trackPointsRight.size();

  trackPointsRight.clear();
  trackPointsRight.reserve( pointsPtr->size() );

  std::copy( pointsPtr->cbegin(), pointsPtr->cend(), std::back_inserter( trackPointsRight ) );

  qDebug() << "simplifyPolylineResultRight" << sizeBefore << trackPointsRight.size();;

  sender()->deleteLater();

  waitForOptimition = !waitForOptimition;

  if( !waitForOptimition ) {
    updateBuffers();
  }
}

int CultivatedAreaMeshGeometry::vertexCount() {
  return m_indexBuffer->data().size() / static_cast<int>( sizeof( uint16_t ) );
}

void CultivatedAreaMeshGeometry::addPoints( const Point_2 pointLeft, const Point_2 pointRight ) {
  addPointLeftWithoutUpdate( pointLeft );
  addPointRightWithoutUpdate( pointRight );

  updateBuffers();
}

void CultivatedAreaMeshGeometry::addPointLeft( const Point_2 point ) {
  addPointLeftWithoutUpdate( point );

  updateBuffers();
}

void CultivatedAreaMeshGeometry::addPointRight( const Point_2 point ) {
  addPointRightWithoutUpdate( point );

  updateBuffers();
}

void CultivatedAreaMeshGeometry::addPointLeftWithoutUpdate( const Point_2 point ) {
  if( !trackPointsLeft.empty() ) {
    if( CGAL::squared_distance( point, trackPointsLeft.back() ) > 0.0001 ) {
      trackPointsLeft.push_back( point );
      updateBuffers();
    }
  } else {
    trackPointsLeft.push_back( point );
  }
}

void CultivatedAreaMeshGeometry::addPointRightWithoutUpdate( const Point_2 point ) {
  if( !trackPointsRight.empty() ) {
    if( CGAL::squared_distance( point, trackPointsRight.back() ) > 0.0001 ) {
      trackPointsRight.push_back( point );
      updateBuffers();
    }
  } else {
    trackPointsRight.push_back( point );
  }
}

#include "moc_CultivatedAreaMeshGeometry.cpp"
