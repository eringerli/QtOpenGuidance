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

#include "CgalWorker.h"
#include "../cgal.h"

#include <QScopedPointer>

CgalWorker::CgalWorker( QObject* parent ) : QObject( parent ) {

}

void CgalWorker::alphaToPolygon( const Alpha_shape_2& A, Polygon_with_holes_2& out_poly ) {
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
  for( auto it = v_edges_map.cbegin(), end = v_edges_map.cend(); it != end; ++it ) {
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

void CgalWorker::fieldOptimitionWorker( uint32_t runNumber,
                                        std::vector<Point_2>* pointsPointer,
                                        FieldsOptimitionToolbar::AlphaType alphaType,
                                        double customAlpha,
                                        double maxDeviation,
                                        double distanceBetweenConnectPoints ) {

  QScopedPointer<std::vector<Point_2>> points( pointsPointer );

  auto cgalThread = qobject_cast<CgalThread*>( thread() );

  if( cgalThread != nullptr ) {
    QMutexLocker lock( &cgalThread->mutex );

    if( runNumber < cgalThread->runNumber ) {
      qDebug() << "Returned early...";
      return;
    }
  }

  qDebug() << "CgalWorker::fieldOptimitionWorker" << points;

  double numPointsRecorded = double( points->size() );

  // check for collinearity: if all points are collinear, you can't calculate a triangulation and it crashes
  bool collinearity = true;
  {
    const std::size_t numPoints = points->size();

    for( std::size_t i = 0; i < ( numPoints - 2 ); ++i ) {
      collinearity &= CGAL::collinear( points->at( i + 0 ), points->at( i + 1 ), points->at( i + 2 ) );

      if( !collinearity ) {
        break;
      }
    }
  }

  if( !collinearity ) {

    if( distanceBetweenConnectPoints > 0 ) {
      qDebug() << "pointsCopy.size()" << points->size();

      for( auto last = points->cbegin(), it = points->cbegin() + 1, end = points->cend();
           it != end;
           ++it, ++last ) {
        const double distance = std::sqrt( CGAL::squared_distance( *last, *it ) );
        std::cout << *last << " " << *it << " " << distance << std::endl << std::flush;

        if( ( distance - 0.01 ) > distanceBetweenConnectPoints ) {
          std::size_t numPoints = std::size_t( distance / distanceBetweenConnectPoints );

          if( numPoints > 1000 ) {
            CGAL::set_pretty_mode( std::cerr );
            std::cerr << "pointsCopy.size(): " << points->size() << " distance: " << distance  << " distanceBetweenConnectPoints:" << distanceBetweenConnectPoints << " numPoints:" << numPoints  << " points: " << *last << " " << *it << "\n";
            numPoints = 10000;
          }

          CGAL::Points_on_segment_2< K::Point_2 > pointGenerator( *last, *it, numPoints );

          for( std::size_t i = 1; i < ( numPoints - 1 ); ++i ) {
            points->push_back( *pointGenerator );
            ++pointGenerator;
          }

          std::cout << "pointsCopy.size()" << points->size() << "distance" << distance << distanceBetweenConnectPoints << numPoints;
        }
      }
    }

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        qDebug() << "Returned early...";
        return;
      }
    }

    qDebug() << "pointsCopy.size()" << points->size();


    Alpha_shape_2 alphaShape( points->begin(), points->end(),
                              K::FT( 0 ),
                              Alpha_shape_2::REGULARIZED );

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        qDebug() << "Returned early...";
        return;
      }
    }

    qDebug() << "Alpha Shape computed";
    double optimalAlpha = CGAL::to_double( *alphaShape.find_optimal_alpha( 1 ) );
    qDebug() << "Optimal alpha: " << optimalAlpha;
    double solidAlpha = CGAL::to_double( alphaShape.find_alpha_solid() );
    qDebug() << "Solid alpha: " << solidAlpha;

    emit alphaChanged( optimalAlpha, solidAlpha );

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        qDebug() << "Returned early...";
        return;
      }
    }

    if( 0 ) {
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
      qDebug() << "alpha shape 2 edges tot: " << numSegments << "points: " << points->size();
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

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        qDebug() << "Returned early...";
        return;
      }
    }

    if( 0 ) {
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
      qDebug() << "alpha shape edges tot: " << numSegments << "points: " << points->size();
      qDebug() << "Ext:" << numSegmentsExterior << "Sin:" << numSegmentsSingular << "Reg:" << numSegmentsRegular << "Int:" << numSegmentsInterior ;
    }

    auto out_poly = new Polygon_with_holes_2();

    alphaToPolygon( alphaShape, *out_poly );

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        return;
      }
    }

    PS::Squared_distance_cost cost;

    *out_poly = PS::simplify( *out_poly, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

    if( cgalThread != nullptr ) {
      QMutexLocker lock( &cgalThread->mutex );

      if( runNumber < cgalThread->runNumber ) {
        qDebug() << "Returned early...";
        return;
      }
    }

    // traverse the vertices and the edges
    {
//      using VertexIterator = Polygon_2::Vertex_iterator;
      CGAL::set_pretty_mode( std::cout );

      qDebug() << "out_poly 2:" << out_poly->outer_boundary().size();

      emit fieldStatisticsChanged( numPointsRecorded, double( points->size() ), double( out_poly->outer_boundary().size() ) );
    }

    emit alphaShapeFinished( std::shared_ptr<Polygon_with_holes_2>( out_poly ), CGAL::to_double( alphaShape.get_alpha() ) );
  }
}
