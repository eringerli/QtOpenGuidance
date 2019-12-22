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

void GlobalPlanner::createPlanAB() {
  if( !qIsNull( abLine.length() ) ) {

    QVector<QVector3D> linePoints;

    // extend the points 200m in either direction
    qreal headingOfABLine = abLine.angle();

    QLineF lineExtensionFromA = QLineF::fromPolar( -200, headingOfABLine );
    lineExtensionFromA.translate( aPoint );

    QLineF lineExtensionFromB = QLineF::fromPolar( 200, headingOfABLine );
    lineExtensionFromB.translate( bPoint );

    QLineF pathLine( lineExtensionFromA.p2(), lineExtensionFromB.p2() );

    QVector<QSharedPointer<PathPrimitive>> plan;

    // the lines are generated to follow in both directions
    if( forwardPasses == 0 && reversePasses == 0 ) {
      // left side
      for( uint16_t i = 0; i < pathsToGenerate; ++i ) {
        plan.append( QSharedPointer<PathPrimitive>(
                       new PathPrimitiveLine(
                         pathLine.translated(
                           QLineF::fromPolar( i * implementLine.dy() +
                                              implementLine.center().y(),
                                              headingOfABLine - 90 ).p2() ), implementLine.dy(), false, true, i ) ) );
      }

      // right side
      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {
        plan.append( QSharedPointer<PathPrimitive>(
                       new PathPrimitiveLine(
                         pathLine.translated(
                           QLineF::fromPolar( i * implementLine.dy() +
                                              implementLine.center().y(),
                                              headingOfABLine - 270 ).p2() ), implementLine.dy(), false, true, -i ) ) );
      }

      // these lines are to follow in the generated direction
    } else {
      // add center line
      auto linePrimitive = new PathPrimitiveLine(
        pathLine.translated(
          QLineF::fromPolar( implementLine.center().y(),
                             headingOfABLine + 90 ).p2() ), implementLine.dy(), false, false, 0 );
      plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );

      enum class PathGenerationStates : uint8_t {
        Forward = 0,
        Reverse,
        ForwardMirrored,
        ReverseMirrored
      } pathGenerationStates = PathGenerationStates::Forward;

      uint16_t passCounter = 1;

      // left side
      if( !startRight ) {
        if( forwardPasses == 1 ) {
          pathGenerationStates = PathGenerationStates::Reverse;
        } else {
          pathGenerationStates = PathGenerationStates::Forward;
        }
      } else {
        if( mirror ) {
          pathGenerationStates = PathGenerationStates::ForwardMirrored;
        } else {
          pathGenerationStates = PathGenerationStates::Reverse;
          passCounter = 0;
        }

      }

      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {

        auto linePrimitive = new PathPrimitiveLine(
          pathLine.translated(
            QLineF::fromPolar( i * implementLine.dy() +
                               implementLine.center().y(),
                               headingOfABLine + 90 ).p2() ), implementLine.dy(), false, false, i );

        ++passCounter;

        // state machine to generate the directions of the lines
        switch( pathGenerationStates ) {
          case PathGenerationStates::Forward:
            if( passCounter >= forwardPasses ) {
              pathGenerationStates = PathGenerationStates::Reverse;
              passCounter = 0;
            }

            break;

          case PathGenerationStates::Reverse:
            linePrimitive->reverse();

            if( passCounter >= reversePasses ) {
              if( mirror ) {
                pathGenerationStates = PathGenerationStates::ReverseMirrored;
              } else {
                pathGenerationStates = PathGenerationStates::Forward;
              }

              passCounter = 0;
            }

            break;

          case PathGenerationStates::ForwardMirrored:
            linePrimitive->reverse();

            if( passCounter >= forwardPasses ) {
              pathGenerationStates = PathGenerationStates::Forward;
              passCounter = 0;
            }

            break;

          case PathGenerationStates::ReverseMirrored:
            if( passCounter >= reversePasses ) {
              if( mirror ) {
                pathGenerationStates = PathGenerationStates::ForwardMirrored;
              } else {
                pathGenerationStates = PathGenerationStates::Forward;
              }

              passCounter = 0;
            }

            break;
        }

        plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );
      }


      // right side
      passCounter = 1;

      if( startRight ) {
        if( forwardPasses == 1 ) {
          pathGenerationStates = PathGenerationStates::Reverse;
        } else {
          pathGenerationStates = PathGenerationStates::Forward;
        }
      } else {
        if( mirror ) {
          pathGenerationStates = PathGenerationStates::ForwardMirrored;
        } else {
          pathGenerationStates = PathGenerationStates::Reverse;
        }

        passCounter = 0;
      }

      for( uint16_t i = 1; i < pathsToGenerate; ++i ) {

        auto linePrimitive = new PathPrimitiveLine(
          pathLine.translated(
            QLineF::fromPolar( i * implementLine.dy() +
                               implementLine.center().y(),
                               headingOfABLine - 90 ).p2() ), implementLine.dy(), false, false, i );

        ++passCounter;

        // state machine to generate the directions of the lines
        switch( pathGenerationStates ) {
          case PathGenerationStates::Forward:
            if( passCounter >= forwardPasses ) {
              pathGenerationStates = PathGenerationStates::Reverse;
              passCounter = 0;
            }

            break;

          case PathGenerationStates::Reverse:
            linePrimitive->reverse();

            if( passCounter >= reversePasses ) {
              if( mirror ) {
                pathGenerationStates = PathGenerationStates::ReverseMirrored;
              } else {
                pathGenerationStates = PathGenerationStates::Forward;
              }

              passCounter = 0;
            }

            break;

          case PathGenerationStates::ForwardMirrored:
            linePrimitive->reverse();

            if( passCounter >= forwardPasses ) {
              pathGenerationStates = PathGenerationStates::Forward;
              passCounter = 0;
            }

            break;

          case PathGenerationStates::ReverseMirrored:
            if( passCounter >= reversePasses ) {
              if( mirror ) {
                pathGenerationStates = PathGenerationStates::ForwardMirrored;
              } else {
                pathGenerationStates = PathGenerationStates::Forward;
              }

              passCounter = 0;
            }

            break;
        }

        plan.append( QSharedPointer<PathPrimitive>( linePrimitive ) );
      }

    }

    emit planChanged( plan );
  }
}

void GlobalPlanner::alphaToPolygon( const Alpha_shape_2& A, Polygon_with_holes_2& out_poly ) {
  typedef typename Alpha_shape_2::Vertex_handle Vertex_handle;
  typedef typename Alpha_shape_2::Edge Edge;
  typedef std::vector<Edge> EdgeVector;

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

  for( auto it = v_edges_map.begin(); it != v_edges_map.end(); ++it ) {
    if( existing_edges.count( ( *it ).second.front() ) ) {
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

void GlobalPlanner::alphaShape() {
  QElapsedTimer timer;
  timer.start();

  std::vector<K::Point_2> pointsCopy( points );

  double distance = CGAL::squared_distance( points.front(), points.back() );
  distance = sqrt( distance );
  distance *= 10;
  std::size_t numPoints = std::size_t( distance );
  CGAL::Points_on_segment_2< K::Point_2 > pointGenerator( points.front(), points.back(), numPoints );
  pointsCopy.reserve( pointsCopy.size() + numPoints );

  for( std::size_t i = 0; i < numPoints; ++i ) {
    pointsCopy.push_back( *pointGenerator );
    ++pointGenerator;
  }

  qDebug() << "points.size()" << points.size() << "pointsCopy.size()" << pointsCopy.size() << "distance" << distance << std::size_t( distance );

  // copy all the points to the mesh
  {
    QVector<QVector3D> meshPoints;

    for( auto&& point : pointsCopy ) {
      meshPoints << QVector3D( float( point.x() ), float( point.y() ), 0 );
    }

    m_pointsMesh->posUpdate( meshPoints );
  }

  Alpha_shape_2 alphaShape( pointsCopy.begin(), pointsCopy.end(),
                            K::FT( 0 ),
                            Alpha_shape_2::REGULARIZED );

  qDebug() << "Alpha Shape computed";
  double optimalAlpha = *alphaShape.find_optimal_alpha( 1 );
  qDebug() << "Optimal alpha: " << optimalAlpha;
  double solidAlpha = alphaShape.find_alpha_solid();
  qDebug() << "Solid alpha: " << solidAlpha;

  emit alphaChanged( optimalAlpha, solidAlpha );

  {
    int numSegments = 0;
    int numSegmentsExterior = 0;
    int numSegmentsSingular = 0;
    int numSegmentsRegular = 0;
    int numSegmentsInterior = 0;

    {
      QVector<QVector3D> meshSegmentPoints;

      Alpha_shape_edges_iterator it = alphaShape.alpha_shape_edges_begin(),
                                 end = alphaShape.alpha_shape_edges_end();

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

        //  if(A.classify(*it) == Alpha_shape_2::EXTERIOR){
        K::Segment_2 segment = alphaShape.segment( *it );
        meshSegmentPoints << QVector3D( float( segment.source().x() ), float( segment.source().y() ), 0.3f );
        meshSegmentPoints << QVector3D( float( segment.target().x() ), float( segment.target().y() ), 0.3f );
        ++numSegments;
        //  }
      }

      m_segmentsMesh2->posUpdate( meshSegmentPoints );
    }
    qDebug() << "alpha shape 2 edges tot: " << numSegments << "points: " << pointsCopy.size();
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
      QVector<QVector3D> meshSegmentPoints;

      Alpha_shape_edges_iterator it = alphaShape.alpha_shape_edges_begin(),
                                 end = alphaShape.alpha_shape_edges_end();

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

        //  if(A.classify(*it) == Alpha_shape_2::EXTERIOR){
        K::Segment_2 segment = alphaShape.segment( *it );

        meshSegmentPoints << QVector3D( float( segment.source().x() ), float( segment.source().y() ), 0.4f );
        meshSegmentPoints << QVector3D( float( segment.target().x() ), float( segment.target().y() ), 0.4f );
        ++numSegments;
        //  }
      }

      m_segmentsMesh->posUpdate( meshSegmentPoints );
    }
    qDebug() << "alpha shape edges tot: " << numSegments << "points: " << pointsCopy.size();
    qDebug() << "Ext:" << numSegmentsExterior << "Sin:" << numSegmentsSingular << "Reg:" << numSegmentsRegular << "Int:" << numSegmentsInterior ;
  }

  Polygon_with_holes_2 out_poly;
  alphaToPolygon( alphaShape, out_poly );

  // traverse the vertices and the edges
  {
    typedef Polygon_2::Vertex_iterator VertexIterator;
    CGAL::set_pretty_mode( std::cout );
    QVector<QVector3D> meshSegmentPoints;

    for( VertexIterator vi = out_poly.outer_boundary().vertices_begin(); vi != out_poly.outer_boundary().vertices_end(); ++vi ) {
      //          std::cout << "vertex " << n++ << " = " << *vi << std::endl;
      meshSegmentPoints << QVector3D( vi->x(), vi->y(), 0.1f );
    }

    meshSegmentPoints << meshSegmentPoints.first();
    m_segmentsMesh3->posUpdate( meshSegmentPoints );
    qDebug() << "out_poly 1:" << meshSegmentPoints.size();
    m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );

    //        std::cout << std::endl;
    //        n = 0;
    //         typedef Polygon_2::Edge_const_iterator EdgeIterator;
    //
    //        for( EdgeIterator ei = out_poly.outer_boundary().edges_begin(); ei != out_poly.outer_boundary().edges_end(); ++ei ) {
    //          std::cout << "edge " << n++ << " = " << *ei << std::endl;
    //        }
  }

  PS::Squared_distance_cost cost;

  out_poly = PS::simplify( out_poly, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

  // traverse the vertices and the edges
  {
    typedef Polygon_2::Vertex_iterator VertexIterator;
    //        typedef Polygon_2::Edge_const_iterator EdgeIterator;
    CGAL::set_pretty_mode( std::cout );
    //        int n = 0;
    QVector<QVector3D> meshSegmentPoints;

    for( VertexIterator vi = out_poly.outer_boundary().vertices_begin(); vi != out_poly.outer_boundary().vertices_end(); ++vi ) {
      //          std::cout << "vertex " << n++ << " = " << *vi << std::endl;
      meshSegmentPoints << QVector3D( vi->x(), vi->y(), 0.1f );
    }

    meshSegmentPoints << meshSegmentPoints.first();
    m_segmentsMesh4->posUpdate( meshSegmentPoints );
    qDebug() << "out_poly 2:" << meshSegmentPoints.size();
    emit fieldStatisticsChanged( points.size(), meshSegmentPoints.size() );
    m_segmentsMesh4->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );

    //        std::cout << std::endl;
    //        n = 0;

    //        for( EdgeIterator ei = out_poly.outer_boundary().edges_begin(); ei != out_poly.outer_boundary().edges_end(); ++ei ) {
    //          std::cout << "edge " << n++ << " = " << *ei << std::endl;
    //        }
  }

  qDebug() << "Time elapsed: " << timer.elapsed() << "ms";
}
