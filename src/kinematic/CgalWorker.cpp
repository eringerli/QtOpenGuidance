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

#include <ThreadWeaver/ThreadWeaver>

#include "CgalWorker.h"
#include "kinematic/cgal.h"

#include <CGAL/point_generators_2.h>

#include <CGAL/Polyline_simplification_2/simplify.h>
namespace PS = CGAL::Polyline_simplification_2;

CgalWorker::CgalWorker( QObject* parent ) : QObject( parent ) {}

void
CgalWorker::alphaToPolygon( const Alpha_shape_2& A, Polygon_with_holes_2& out_poly ) {
  using Vertex_handle = typename Alpha_shape_2::Vertex_handle;
  using Edge          = typename Alpha_shape_2::Edge;
  using EdgeVector    = std::vector< Edge >;

  // form vertex to vertex map
  std::map< Vertex_handle, EdgeVector > v_edges_map;

  for( auto it = A.alpha_shape_edges_begin(); it != A.alpha_shape_edges_end(); ++it ) {
    auto      edge = *it; // edge <=> pair<face_handle, vertex id>
    const int vid  = edge.second;
    auto      v    = edge.first->vertex( ( vid + 1 ) % 3 );

    if( v_edges_map.count( v ) == 0 ) {
      v_edges_map[v] = EdgeVector();
    }

    v_edges_map[v].push_back( edge );
  }

  // form all possible boundaries
  std::vector< Polygon_2 > polies;
  std::vector< double >    lengths;
  double                   max_length = 0;
  std::size_t              max_id     = 0;
  std::set< Edge >         existing_edges;

  for( auto it = v_edges_map.cbegin(), end = v_edges_map.cend(); it != end; ++it ) {
    if( existing_edges.count( ( *it ).second.front() ) != 0u ) {
      continue;
    }

    auto begin_v = ( *it ).first;
    auto next_e  = ( *it ).second.front();
    auto next_v  = next_e.first->vertex( ( next_e.second + 2 ) % 3 );

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
      max_id     = polies.size();
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

bool
CgalWorker::returnEarly( uint32_t runNumber ) {
  auto* cgalThread = qobject_cast< CgalThread* >( thread() );

  if( cgalThread != nullptr ) {
    QMutexLocker lock( &cgalThread->mutex );

    if( runNumber < cgalThread->runNumber ) {
      return true;
    }
  }

  return false;
}

bool
CgalWorker::isCollinear( std::vector< Point_2 >* pointsPointer, bool emitSignal ) {
  bool collinearity = true;

  for( std::size_t i = 0, end = pointsPointer->size(); i < ( end - 2 ); ++i ) {
    collinearity &= CGAL::collinear( pointsPointer->at( i + 0 ), pointsPointer->at( i + 1 ), pointsPointer->at( i + 2 ) );

    if( !collinearity ) {
      break;
    }
  }

  if( emitSignal ) {
    Q_EMIT isCollinearResult( collinearity );
  }

  return collinearity;
}

void
CgalWorker::connectPoints( std::vector< Point_2 >* pointsPointer, double distanceBetweenConnectPoints, bool emitSignal ) {
  if( distanceBetweenConnectPoints > 0 && pointsPointer->size() >= 2 ) {
    for( auto last = pointsPointer->cbegin(), it = pointsPointer->cbegin() + 1, end = pointsPointer->cend(); it != end; ++it, ++last ) {
      const double distance = std::sqrt( CGAL::squared_distance( *last, *it ) );

      if( ( distance - 0.01 ) > distanceBetweenConnectPoints ) {
        auto numPoints = std::size_t( distance / distanceBetweenConnectPoints );

        if( numPoints > 1000 ) {
          numPoints = 1000;
        }

        CGAL::Points_on_segment_2< Epick::Point_2 > pointGenerator( *last, *it, numPoints );

        for( std::size_t i = 1; i < ( numPoints - 1 ); ++i ) {
          pointsPointer->push_back( *pointGenerator );
          ++pointGenerator;
        }
      }
    }
  }

  if( emitSignal ) {
    Q_EMIT connectPointsResult( pointsPointer );
  }
}

void
CgalWorker::simplifyPolygon( Polygon_with_holes_2* out_poly, double maxDeviation, bool emitSignal ) {
  PS::Squared_distance_cost cost;

  *out_poly = PS::simplify( *out_poly, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

  if( emitSignal ) {
    Q_EMIT simplifyPolygonResult( out_poly );
  }
}

void
CgalWorker::simplifyPolyline_2( std::vector< Point_2 >* pointsPointer, double maxDeviation ) {
  PS::Squared_distance_cost cost;

  auto* polylineOut = new std::vector< Point_2 >;

  PS::simplify( pointsPointer->begin(),
                pointsPointer->end(),
                cost,
                PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ),
                std::back_inserter( *polylineOut ) );

  Q_EMIT simplifyPolylineResult_2( polylineOut );
}

#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Projection_traits_xz_3.h>
typedef CGAL::Projection_traits_xy_3< Epick > Epick_3_2xy;
typedef CGAL::Projection_traits_xz_3< Epick > Epick_3_2xz;

typedef CGAL::Polygon_2< Epick_3_2xy >                                                                   Polygon_3_2xy;
typedef PS::Vertex_base_2< Epick_3_2xy >                                                                 Vb_3_2xy;
typedef CGAL::Constrained_triangulation_face_base_2< Epick_3_2xy >                                       Fb_3_2xy;
typedef CGAL::Triangulation_data_structure_2< Vb_3_2xy, Fb_3_2xy >                                       TDS_3_2xy;
typedef CGAL::Constrained_Delaunay_triangulation_2< Epick_3_2xy, TDS_3_2xy, CGAL::Exact_predicates_tag > CDT_3_2xy;
typedef CGAL::Constrained_triangulation_plus_2< CDT_3_2xy >                                              CT_3_2xy;

void
CgalWorker::simplifyPolyline_3( std::vector< Point_3 >* pointsPointer, double maxDeviation ) {
  PS::Squared_distance_cost cost;

  CT_3_2xy      ct;
  Polygon_3_2xy polygon( pointsPointer->cbegin(), pointsPointer->cend() );
  ct.insert_constraint( polygon );

  auto* polylineOut = new std::vector< Point_3 >;

  PS::simplify( ct, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

  for( auto cit = ct.constraints_begin(), citend = ct.constraints_end(); cit != citend; ++cit ) {
    //    std::cout << "simplified polyline" << std::endl;
    for( auto vit = ct.points_in_constraint_begin( *cit ), vitend = ct.points_in_constraint_end( *cit ); vit != vitend; ++vit ) {
      //      std::cout << *vit << std::endl;
      polylineOut->push_back( *vit );
    }
  }

  Q_EMIT simplifyPolylineResult_3( polylineOut );
}

void
CgalWorker::fieldOptimitionWorker( const uint32_t                           runNumber,
                                   std::vector< Point_2 >*                  pointsPointer,
                                   const FieldsOptimitionToolbar::AlphaType alphaType,
                                   const double                             customAlpha,
                                   const double                             maxDeviation,
                                   const double                             distanceBetweenConnectPoints ) {
  QScopedPointer< std::vector< Point_2 > > points( pointsPointer );

  if( returnEarly( runNumber ) ) {
    return;
  }

  auto numPointsRecorded = double( points->size() );

  // check for collinearity: if all points are collinear, you can't calculate a
  // triangulation and it crashes
  if( !isCollinear( pointsPointer ) ) {
    connectPoints( pointsPointer, distanceBetweenConnectPoints );

    if( returnEarly( runNumber ) ) {
      return;
    }

    Alpha_shape_2 alphaShape( points->begin(), points->end(), Epick::FT( 0 ), Alpha_shape_2::REGULARIZED );

    if( returnEarly( runNumber ) ) {
      return;
    }

    double optimalAlpha = CGAL::to_double( *alphaShape.find_optimal_alpha( 1 ) );
    double solidAlpha   = CGAL::to_double( alphaShape.find_alpha_solid() );

    Q_EMIT alphaChanged( optimalAlpha, solidAlpha );

    if( returnEarly( runNumber ) ) {
      return;
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

    if( returnEarly( runNumber ) ) {
      return;
    }

    auto* out_poly = new Polygon_with_holes_2();

    alphaToPolygon( alphaShape, *out_poly );

    if( returnEarly( runNumber ) ) {
      return;
    }

    simplifyPolygon( out_poly, maxDeviation );

    if( returnEarly( runNumber ) ) {
      return;
    }

    Q_EMIT fieldStatisticsChanged( numPointsRecorded, double( points->size() ), double( out_poly->outer_boundary().size() ) );

    Q_EMIT alphaShapeFinished( std::shared_ptr< Polygon_with_holes_2 >( out_poly ), CGAL::to_double( alphaShape.get_alpha() ) );
  }
}

// Without this include, qmake creates a rule to compile moc_CgalWorker.cpp standalone,
// which doesn't include cgal.h for performance reasons
#include "moc_CgalWorker.cpp"
