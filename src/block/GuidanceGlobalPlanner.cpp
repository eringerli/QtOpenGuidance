// Copyright( C ) 2019 Christian Riggenbach
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

#include "GuidanceGlobalPlanner.h"
#include <QtConcurrent>

#include "../cgal.h"

void GlobalPlanner::createPlanAB() {
//  if( !qIsNull( abLine.squared_lenght() ) ) {

//    QVector<QVector3D> linePoints;

//    // extend the points 200m in either direction
//    qreal headingOfABLine = abLine.angle();

//    QLineF lineExtensionFromA = QLineF::fromPolar( -200, headingOfABLine );
//    lineExtensionFromA.translate( aPoint );

//    QLineF lineExtensionFromB = QLineF::fromPolar( 200, headingOfABLine );
//    lineExtensionFromB.translate( bPoint );

//    QLineF pathLine( lineExtensionFromA.p2(), lineExtensionFromB.p2() );

//    QVector<QSharedPointer<PathPrimitive>> plan;

//    // the lines are generated to follow in both directions
//    if( forwardPasses == 0 && reversePasses == 0 ) {
//      // left side
//      for( uint16_t i = 0; i < pathsToGenerate; ++i ) {
//        plan.append( QSharedPointer<PathPrimitive>(
//                       new PathPrimitiveLine(
//                         pathLine.translated(
//                           QLineF::fromPolar( i * implementLine.dy() +
//                                              implementLine.center().y(),
//                                              headingOfABLine - 90 ).p2() ), implementLine.dy(), false, true, i ) ) );
//      }

//      // right side
//      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {
//        plan.append( QSharedPointer<PathPrimitive>(
//                       new PathPrimitiveLine(
//                         pathLine.translated(
//                           QLineF::fromPolar( i * implementLine.dy() +
//                                              implementLine.center().y(),
//                                              headingOfABLine - 270 ).p2() ), implementLine.dy(), false, true, -i ) ) );
//      }

//      // these lines are to follow in the generated direction
//    } else {
//      // add center line
//      auto linePrimitive = new PathPrimitiveLine(
//        pathLine.translated(
//          QLineF::fromPolar( implementLine.center().y(),
//                             headingOfABLine + 90 ).p2() ), implementLine.dy(), false, false, 0 );
//      plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );

//      enum class PathGenerationStates : uint8_t {
//        Forward = 0,
//        Reverse,
//        ForwardMirrored,
//        ReverseMirrored
//      } pathGenerationStates = PathGenerationStates::Forward;

//      uint16_t passCounter = 1;

//      // left side
//      if( !startRight ) {
//        if( forwardPasses == 1 ) {
//          pathGenerationStates = PathGenerationStates::Reverse;
//        } else {
//          pathGenerationStates = PathGenerationStates::Forward;
//        }
//      } else {
//        if( mirror ) {
//          pathGenerationStates = PathGenerationStates::ForwardMirrored;
//        } else {
//          pathGenerationStates = PathGenerationStates::Reverse;
//          passCounter = 0;
//        }

//      }

//      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {

//        auto linePrimitive = new PathPrimitiveLine(
//          pathLine.translated(
//            QLineF::fromPolar( i * implementLine.dy() +
//                               implementLine.center().y(),
//                               headingOfABLine + 90 ).p2() ), implementLine.dy(), false, false, i );

//        ++passCounter;

//        // state machine to generate the directions of the lines
//        switch( pathGenerationStates ) {
//          case PathGenerationStates::Forward:
//            if( passCounter >= forwardPasses ) {
//              pathGenerationStates = PathGenerationStates::Reverse;
//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::Reverse:
//            linePrimitive->reverse();

//            if( passCounter >= reversePasses ) {
//              if( mirror ) {
//                pathGenerationStates = PathGenerationStates::ReverseMirrored;
//              } else {
//                pathGenerationStates = PathGenerationStates::Forward;
//              }

//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::ForwardMirrored:
//            linePrimitive->reverse();

//            if( passCounter >= forwardPasses ) {
//              pathGenerationStates = PathGenerationStates::Forward;
//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::ReverseMirrored:
//            if( passCounter >= reversePasses ) {
//              if( mirror ) {
//                pathGenerationStates = PathGenerationStates::ForwardMirrored;
//              } else {
//                pathGenerationStates = PathGenerationStates::Forward;
//              }

//              passCounter = 0;
//            }

//            break;
//        }

//        plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );
//      }


//      // right side
//      passCounter = 1;

//      if( startRight ) {
//        if( forwardPasses == 1 ) {
//          pathGenerationStates = PathGenerationStates::Reverse;
//        } else {
//          pathGenerationStates = PathGenerationStates::Forward;
//        }
//      } else {
//        if( mirror ) {
//          pathGenerationStates = PathGenerationStates::ForwardMirrored;
//        } else {
//          pathGenerationStates = PathGenerationStates::Reverse;
//        }

//        passCounter = 0;
//      }

//      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {

//        auto linePrimitive = new PathPrimitiveLine(
//          pathLine.translated(
//            QLineF::fromPolar( i * implementLine.dy() +
//                               implementLine.center().y(),
//                               headingOfABLine - 90 ).p2() ), implementLine.dy(), false, false, i );

//        ++passCounter;

//        // state machine to generate the directions of the lines
//        switch( pathGenerationStates ) {
//          case PathGenerationStates::Forward:
//            if( passCounter >= forwardPasses ) {
//              pathGenerationStates = PathGenerationStates::Reverse;
//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::Reverse:
//            linePrimitive->reverse();

//            if( passCounter >= reversePasses ) {
//              if( mirror ) {
//                pathGenerationStates = PathGenerationStates::ReverseMirrored;
//              } else {
//                pathGenerationStates = PathGenerationStates::Forward;
//              }

//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::ForwardMirrored:
//            linePrimitive->reverse();

//            if( passCounter >= forwardPasses ) {
//              pathGenerationStates = PathGenerationStates::Forward;
//              passCounter = 0;
//            }

//            break;

//          case PathGenerationStates::ReverseMirrored:
//            if( passCounter >= reversePasses ) {
//              if( mirror ) {
//                pathGenerationStates = PathGenerationStates::ForwardMirrored;
//              } else {
//                pathGenerationStates = PathGenerationStates::Forward;
//              }

//              passCounter = 0;
//            }

//            break;
//        }

//        plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );
//      }

//    }

//    emit planChanged( plan );
//  }
}

void GlobalPlanner::alphaToPolygon( const Alpha_shape_2& A, Polygon_with_holes_2& out_poly ) {
  using Vertex_handle = typename Alpha_shape_2::Vertex_handle;
  using Edge = typename Alpha_shape_2::Edge;
  using EdgeVector = std::vector<Edge>;

  // form vertex to vertex map
  std::map<Vertex_handle, EdgeVector> v_edges_map;

  for( auto it = A.alpha_shape_edges_begin(); it != A.alpha_shape_edges_end(); ++it ) {
    auto edge = *it;  // edge <=> pair<face_handle, vertex id>
    const int vid = edge.second;
    auto v = edge.first->vertex( ( vid + 1 ) % 3 );

    if( v_edges_map.count( v ) == 0 ) {
      v_edges_map[v] = EdgeVector();
    }

    v_edges_map[v].push_back( edge );
  }

  // form all possible boundaries
  std::vector<Polygon_2> polies;
  std::vector<double> lengths;
  double max_length = 0;
  std::size_t max_id = 0;
  std::set<Edge> existing_edges;

//  for( const auto& edge : v_edges_map ) {
  for( auto it = v_edges_map.cbegin(); it != v_edges_map.cend(); ++it ) {
    if( existing_edges.count( ( *it ).second.front() ) != 0u ) {
      continue;
    }

    auto begin_v = ( *it ).first;
    auto next_e = ( *it ).second.front();
    auto next_v = next_e.first->vertex( ( next_e.second + 2 ) % 3 );

    Polygon_2 poly;
    poly.push_back( next_v->point() );

    existing_edges.insert( next_e );

    if( v_edges_map[begin_v].size() > 1 ) {
      v_edges_map[begin_v].erase( v_edges_map[begin_v].begin() );
    }

    double length = CGAL::squared_distance( begin_v->point(), next_v->point() );

    while( next_v != begin_v && v_edges_map.count( next_v ) > 0 ) {
      next_e = v_edges_map[next_v].front();
      existing_edges.insert( next_e );

      if( v_edges_map[next_v].size() > 1 ) {
        v_edges_map[next_v].erase( v_edges_map[next_v].begin() );
      }

      auto t_next_v = next_e.first->vertex( ( next_e.second + 2 ) % 3 );
      length += CGAL::squared_distance( next_v->point(), t_next_v->point() );
      next_v = t_next_v;
      poly.push_back( next_v->point() );
    }

    if( max_length < length ) {
      max_length = length;
      max_id = polies.size();
    }

    polies.push_back( poly );
    lengths.push_back( length );
  }

  // build polygon with holes
  // the first one is outer boundary, the rest are holes
  Polygon_2 outer_poly = polies[max_id];
  polies.erase( polies.begin() + max_id );
  out_poly = Polygon_with_holes_2( outer_poly, polies.begin(), polies.end() );
}

GlobalPlanner::GlobalPlanner( Qt3DCore::QEntity* rootEntity, TransverseMercatorWrapper* tmw )
  : BlockBase(),
    tmw( tmw ) {
  // a point marker -> orange
  {
    aPointMesh = new Qt3DExtras::QSphereMesh();
    aPointMesh->setRadius( .2f );
    aPointMesh->setSlices( 20 );
    aPointMesh->setRings( 20 );

    aPointTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( "orange" ) );

    aPointEntity = new Qt3DCore::QEntity( rootEntity );
    aPointEntity->addComponent( aPointMesh );
    aPointEntity->addComponent( material );
    aPointEntity->addComponent( aPointTransform );
    aPointEntity->setEnabled( false );

    aTextEntity = new Qt3DCore::QEntity( aPointEntity );
    Qt3DExtras::QExtrudedTextMesh* aTextMesh = new Qt3DExtras::QExtrudedTextMesh();
    aTextMesh->setText( QStringLiteral( "A" ) );
    aTextMesh->setDepth( 0.05f );

    aTextEntity->setEnabled( true );
    aTextTransform = new Qt3DCore::QTransform();
    aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    aTextTransform->setScale( 2.0f );
    aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    aTextEntity->addComponent( aTextTransform );
    aTextEntity->addComponent( aTextMesh );
    aTextEntity->addComponent( material );
  }

  // b point marker -> purple
  {
    bPointMesh = new Qt3DExtras::QSphereMesh();
    bPointMesh->setRadius( .2f );
    bPointMesh->setSlices( 20 );
    bPointMesh->setRings( 20 );

    bPointTransform = new Qt3DCore::QTransform();

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse( QColor( "purple" ) );

    bPointEntity = new Qt3DCore::QEntity( rootEntity );
    bPointEntity->addComponent( bPointMesh );
    bPointEntity->addComponent( material );
    bPointEntity->addComponent( bPointTransform );
    bPointEntity->setEnabled( false );

    bTextEntity = new Qt3DCore::QEntity( bPointEntity );
    Qt3DExtras::QExtrudedTextMesh* bTextMesh = new Qt3DExtras::QExtrudedTextMesh();
    bTextMesh->setText( QStringLiteral( "B" ) );
    bTextMesh->setDepth( 0.05f );

    bTextEntity->setEnabled( true );
    bTextTransform = new Qt3DCore::QTransform();
    bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    bTextTransform->setScale( 2.0f );
    bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    bTextEntity->addComponent( bTextTransform );
    bTextEntity->addComponent( bTextMesh );
    bTextEntity->addComponent( material );
  }

  // test for recording
  {
    m_baseEntity = new Qt3DCore::QEntity( rootEntity );
    m_baseTransform = new Qt3DCore::QTransform();
    m_baseEntity->addComponent( m_baseTransform );

    m_pointsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity->setEnabled( false );
    m_segmentsEntity2->setEnabled( false );
    m_segmentsEntity3->setEnabled( false );

    m_pointsMesh = new BufferMesh();
    m_pointsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
    m_pointsEntity->addComponent( m_pointsMesh );

    m_segmentsMesh = new BufferMesh();
    m_segmentsEntity->addComponent( m_segmentsMesh );

    m_segmentsMesh2 = new BufferMesh();
    m_segmentsEntity2->addComponent( m_segmentsMesh2 );

    m_segmentsMesh3 = new BufferMesh();
    m_segmentsEntity3->addComponent( m_segmentsMesh3 );

    m_segmentsMesh4 = new BufferMesh();
    m_segmentsEntity4->addComponent( m_segmentsMesh4 );

    m_pointsMaterial = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
    m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
    m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
    m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
    m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

    m_pointsMaterial->setAmbient( Qt::yellow );
    m_segmentsMaterial->setAmbient( Qt::white );
    m_segmentsMaterial2->setAmbient( Qt::green );
    m_segmentsMaterial3->setAmbient( Qt::blue );
    m_segmentsMaterial4->setAmbient( Qt::red );

    m_pointsEntity->addComponent( m_pointsMaterial );
    m_segmentsEntity->addComponent( m_segmentsMaterial );
    m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
    m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
    m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
  }
}

Polygon_with_holes_2* GlobalPlanner::fieldOptimitionWorker( std::vector<Point_2>* points,
    double distanceBetweenConnectPoints,
    FieldsOptimitionToolbar::AlphaType alphaType,
    double customAlpha,
    double maxDeviation ) {
  auto& pointsCopy2D = *points;
  Polygon_with_holes_2* out_poly = nullptr;

  // check for collinearity: if all points are collinear, you can't calculate a triangulation and it crashes
  bool collinearity = true;
  {
    const std::size_t numPoints = pointsCopy2D.size();

    for( std::size_t i = 0; i < ( numPoints - 2 ); ++i ) {
      collinearity &= CGAL::collinear( pointsCopy2D[i + 0], pointsCopy2D[i + 1], pointsCopy2D[i + 2] );

      if( !collinearity ) {
        break;
      }
    }
  }

  if( !collinearity ) {

    if( distanceBetweenConnectPoints > 0 ) {
      qDebug() << "pointsCopy.size()" << pointsCopy2D.size();

      for( auto last = pointsCopy2D.cbegin(), it = pointsCopy2D.cbegin() + 1, end = pointsCopy2D.cend();
           it != end;
           ++it, ++last ) {
        const double distance = CGAL::squared_distance( *last, *it );
        std::cout << *last << " " << *it << " " << distance << std::endl << std::flush;

        if( ( distance - 0.01 ) > distanceBetweenConnectPoints ) {
          std::size_t numPoints = std::size_t( distance / distanceBetweenConnectPoints );
          CGAL::Points_on_segment_2< K::Point_2 > pointGenerator( *last, *it, numPoints );

          for( std::size_t i = 1; i < ( numPoints - 1 ); ++i ) {
            pointsCopy2D.push_back( *pointGenerator );
            ++pointGenerator;
          }

          qDebug() << "pointsCopy.size()" << pointsCopy2D.size() << "distance" << distance << distanceBetweenConnectPoints << numPoints;
        }
      }
    }

    qDebug() << "pointsCopy.size()" << pointsCopy2D.size();


    Alpha_shape_2 alphaShape( pointsCopy2D.begin(), pointsCopy2D.end(),
                              K::FT( 0 ),
                              Alpha_shape_2::REGULARIZED );

    qDebug() << "Alpha Shape computed";
    double optimalAlpha = *alphaShape.find_optimal_alpha( 1 );
    qDebug() << "Optimal alpha: " << optimalAlpha;
    double solidAlpha = alphaShape.find_alpha_solid();
    qDebug() << "Solid alpha: " << solidAlpha;

//    emit alphaChanged( optimalAlpha, solidAlpha );

    {
      int numSegments = 0;
      int numSegmentsExterior = 0;
      int numSegmentsSingular = 0;
      int numSegmentsRegular = 0;
      int numSegmentsInterior = 0;

      {
        auto it = alphaShape.alpha_shape_edges_begin();
        const auto& end = alphaShape.alpha_shape_edges_end();

        for( ; it != end; ++it ) {
          switch( alphaShape.classify( *it ) ) {
            case Alpha_shape_2::EXTERIOR:
              ++numSegmentsExterior;
              break;

            case Alpha_shape_2::SINGULAR:
              ++numSegmentsInterior;
              break;

            case Alpha_shape_2::REGULAR:
              ++numSegmentsRegular;
              break;

            case Alpha_shape_2::INTERIOR:
              ++numSegmentsInterior;
              break;
          }

          ++numSegments;
        }

      }
      qDebug() << "alpha shape 2 edges tot: " << numSegments << "points: " << pointsCopy2D.size();
      qDebug() << "Ext:" << numSegmentsExterior << "Sin:" << numSegmentsSingular << "Reg:" << numSegmentsRegular << "Int:" << numSegmentsInterior ;
    }

    switch( alphaType ) {
      default:
      case FieldsOptimitionToolbar::AlphaType::Optimal:
        alphaShape.set_alpha( optimalAlpha + 0.1 );
        break;

      case FieldsOptimitionToolbar::AlphaType::Solid:
        alphaShape.set_alpha( solidAlpha + 0.1 );
        break;

      case FieldsOptimitionToolbar::AlphaType::Custom:
        alphaShape.set_alpha( customAlpha );
        break;
    }

    {
      int numSegments = 0;
      int numSegmentsExterior = 0;
      int numSegmentsSingular = 0;
      int numSegmentsRegular = 0;
      int numSegmentsInterior = 0;

      {
        auto it = alphaShape.alpha_shape_edges_begin();
        const auto& end = alphaShape.alpha_shape_edges_end();

        for( ; it != end; ++it ) {
          switch( alphaShape.classify( *it ) ) {
            case Alpha_shape_2::EXTERIOR:
              ++numSegmentsExterior;
              break;

            case Alpha_shape_2::SINGULAR:
              ++numSegmentsInterior;
              break;

            case Alpha_shape_2::REGULAR:
              ++numSegmentsRegular;
              break;

            case Alpha_shape_2::INTERIOR:
              ++numSegmentsInterior;
              break;
          }

          ++numSegments;
        }

      }
      qDebug() << "alpha shape edges tot: " << numSegments << "points: " << pointsCopy2D.size();
      qDebug() << "Ext:" << numSegmentsExterior << "Sin:" << numSegmentsSingular << "Reg:" << numSegmentsRegular << "Int:" << numSegmentsInterior ;
    }

    out_poly = new Polygon_with_holes_2();

    GlobalPlanner::alphaToPolygon( alphaShape, *out_poly );

    PS::Squared_distance_cost cost;

    *out_poly = PS::simplify( *out_poly, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

    // traverse the vertices and the edges
    {
//      using VertexIterator = Polygon_2::Vertex_iterator;
      CGAL::set_pretty_mode( std::cout );

      qDebug() << "out_poly 2:" << out_poly->outer_boundary().size();
//      emit fieldStatisticsChanged( pointsCopy2D.size(), size_t( out_poly->outer_boundary().size() ) );
    }
  }

  delete points;

  return out_poly;
}

void GlobalPlanner::alphaShape() {
  auto timer = new QElapsedTimer();
  timer->start();

  // you need at least 3 points for area
  if( points.size() >= 3 ) {

    // make a 2D copy of the recorded points
    auto pointsCopy2D = new std::vector<K::Point_2>();

    for( const auto& point : points ) {
      pointsCopy2D->emplace_back( point.x(), point.y() );
    }

    auto watcher = new QFutureWatcher<Polygon_with_holes_2*>();
    QObject::connect( watcher, &QFutureWatcher<Polygon_with_holes_2*>::finished, this, [timer, watcher, this ]() {
//      try {
      this->alphaShapeFinished( watcher->future().result() );
//      } catch(exception& e){qDebug()<<"                                                                                    EXCEPTION CAUGHT!!!"<<e.what();}
      delete watcher;
      qDebug() << "Time elapsed: " << timer->elapsed() << "ms";
      delete timer;

    } );

    auto future = QtConcurrent::run( &GlobalPlanner::fieldOptimitionWorker, pointsCopy2D, distanceBetweenConnectPoints, alphaType, customAlpha, maxDeviation );
//    watcher->setFuture( future );

  }

}


void GlobalPlanner::alphaShapeFinished( Polygon_with_holes_2* out_poly ) {

  QVector<QVector3D> meshSegmentPoints;
  typedef Polygon_2::Vertex_iterator VertexIterator;

  if( out_poly != nullptr ) {

    for( VertexIterator vi = out_poly->outer_boundary().vertices_begin(),
         end = out_poly->outer_boundary().vertices_end();
         vi != end; ++vi ) {
      meshSegmentPoints << QVector3D( float( vi->x() ), float( vi->y() ), 0.1f );
    }

    meshSegmentPoints << meshSegmentPoints.first();
    m_segmentsMesh4->bufferUpdate( meshSegmentPoints );
  }

  delete out_poly;
}
