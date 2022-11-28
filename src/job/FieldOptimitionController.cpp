// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FieldOptimitionController.h"

#include "ThreadWeaver/DebuggingAids"

#include "job/ConnectPointsJob.h"
#include "job/TestCollinearityJob.h"

#include <CGAL/Polyline_simplification_2/simplify.h>
namespace PS = CGAL::Polyline_simplification_2;

int FieldOptimitionController::counter = 1;

FieldOptimitionController::FieldOptimitionController( std::shared_ptr< std::vector< Point_2 > > pointsPointer )
    : pointsPointer( pointsPointer ) {
  ++counter;
  std::cout << "FieldOptimitionController::FieldOptimitionController " << this << ", " << counter << std::endl;
  ThreadWeaver::setDebugLevel( true, 0 );
}

void
FieldOptimitionController::run( const FieldsOptimitionToolbar::AlphaType alphaType,
                                const double                             customAlpha,
                                const double                             maxDeviation,
                                const double                             distanceBetweenConnectPoints ) {
  std::cout << "FieldOptimitionController::run  " << this << std::endl;

  auto numPointsRecorded = double( pointsPointer->size() );

  auto seqPointer = new ThreadWeaver::Sequence();

  // add the optimition as discrete steps to the threadweaver, so it can be
  // interrupted/aborted early if necessary
  *seqPointer << ThreadWeaver::make_job( new TestCollinearityJob( pointsPointer ) );

  *seqPointer << ThreadWeaver::make_job( new ConnectPointsJob( pointsPointer, distanceBetweenConnectPoints ) );

  *seqPointer << ThreadWeaver::make_job( [=] {
    std::cout << "FieldOptimitionController::job 3 " << this << ", " << counter << std::endl;
    alphaShape =
      std::make_shared< Alpha_shape_2 >( pointsPointer->begin(), pointsPointer->end(), Epick::FT( 0 ), Alpha_shape_2::REGULARIZED );
  } );

  *seqPointer << ThreadWeaver::make_job( [=] {
    //    sleep(2);

    std::cout << "FieldOptimitionController::job 4 " << this << ", " << counter << std::endl;
    optimalAlpha = CGAL::to_double( *( alphaShape->find_optimal_alpha( 1 ) ) );
    solidAlpha   = CGAL::to_double( alphaShape->find_alpha_solid() );

    Q_EMIT alphaChanged( optimalAlpha, solidAlpha );
  } );

  *seqPointer << ThreadWeaver::make_job( [=] {
    std::cout << "FieldOptimitionController::job 5 " << this << ", " << counter << std::endl;

    switch( alphaType ) {
      default:
      case FieldsOptimitionToolbar::AlphaType::Optimal:
        alphaShape->set_alpha( optimalAlpha + 0.1 );
        break;

      case FieldsOptimitionToolbar::AlphaType::Solid:
        alphaShape->set_alpha( solidAlpha + 0.1 );
        break;

      case FieldsOptimitionToolbar::AlphaType::Custom:
        alphaShape->set_alpha( customAlpha );
        break;
    }
  } );

  *seqPointer << ThreadWeaver::make_job( [=] {
    std::cout << "FieldOptimitionController::job 6 " << this << ", " << counter << std::endl;
    alphaToPolygon();
  } );

  *seqPointer << ThreadWeaver::make_job( [=] {
    std::cout << "FieldOptimitionController::job 7 " << this << ", " << counter << std::endl;
    PS::Squared_distance_cost cost;

    *out_poly = PS::simplify( *out_poly, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );
  } );

  *seqPointer << ThreadWeaver::make_job( [=] {
    std::cout << "FieldOptimitionController::job 8 " << this << ", " << counter << ", " << numPointsRecorded + counter << ", "
              << double( pointsPointer->size() ) << ", " << double( out_poly->outer_boundary().size() ) << std::endl;
    Q_EMIT fieldStatisticsChanged(
      numPointsRecorded + counter, double( pointsPointer->size() ), double( out_poly->outer_boundary().size() ) );

    Q_EMIT fieldOptimitionFinished( std::shared_ptr< Polygon_with_holes_2 >( out_poly ), CGAL::to_double( alphaShape->get_alpha() ) );
  } );

  sequence = ThreadWeaver::make_job( seqPointer );

  ThreadWeaver::stream() << sequence;

  std::cout << "FieldOptimitionController::run2 " << this << std::endl;
}

void
FieldOptimitionController::stop() {
  auto seqPointer = sequence.dynamicCast< ThreadWeaver::Sequence >();

  if( seqPointer ) {
    seqPointer->stop( nullptr );
  }
}

FieldOptimitionController::~FieldOptimitionController() {
  auto seqPointer = sequence.dynamicCast< ThreadWeaver::Sequence >();

  if( seqPointer ) {
    seqPointer->stop( nullptr );
  }

  std::cout << "FieldOptimitionController::~FieldOptimitionController " << this << std::endl;
}

void
FieldOptimitionController::alphaToPolygon() {
  using Vertex_handle = typename Alpha_shape_2::Vertex_handle;
  using Edge          = typename Alpha_shape_2::Edge;
  using EdgeVector    = std::vector< Edge >;

  // form vertex to vertex map
  std::map< Vertex_handle, EdgeVector > v_edges_map;

  for( auto it = alphaShape->alpha_shape_edges_begin(); it != alphaShape->alpha_shape_edges_end(); ++it ) {
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
  out_poly = std::make_shared< Polygon_with_holes_2 >( outer_poly, polies.begin(), polies.end() );
}
