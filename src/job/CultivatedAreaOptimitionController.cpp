// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CultivatedAreaOptimitionController.h"

#include <algorithm>

#include "ThreadWeaver/DebuggingAids"
#include <ThreadWeaver/Dependency>
#include <ThreadWeaver/DependencyPolicy>
#include <ThreadWeaver/Exception>

#include "job/ConnectPointsJob.h"
#include "job/TestCollinearityJob.h"
#include <CGAL/Delaunay_triangulation_2.h>

#include <CGAL/Polyline_simplification_2/simplify.h>
namespace PS = CGAL::Polyline_simplification_2;

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

typedef CGAL::Polygon_2< Epick_3_2xz >                                                                   Polygon_3_2xz;
typedef PS::Vertex_base_2< Epick_3_2xz >                                                                 Vb_3_2xz;
typedef CGAL::Constrained_triangulation_face_base_2< Epick_3_2xz >                                       Fb_3_2xz;
typedef CGAL::Triangulation_data_structure_2< Vb_3_2xz, Fb_3_2xz >                                       TDS_3_2xz;
typedef CGAL::Constrained_Delaunay_triangulation_2< Epick_3_2xz, TDS_3_2xz, CGAL::Exact_predicates_tag > CDT_3_2xz;
typedef CGAL::Constrained_triangulation_plus_2< CDT_3_2xz >                                              CT_3_2xz;

CultivatedAreaOptimitionController::CultivatedAreaOptimitionController( std::shared_ptr< std::vector< Point_3 > >   pointsPointer,
                                                                        std::shared_ptr< std::vector< QVector3D > > normalsPointer )
    : pointsPointer( pointsPointer ), normalsPointer( normalsPointer ) {
  std::cout << "CultivatedAreaOptimitionController::CultivatedAreaOptimitionController " << this << std::endl;
  //  ThreadWeaver::setDebugLevel( true, 10 );
}

void
CultivatedAreaOptimitionController::run( const double maxDeviation ) {
  std::cout << "CultivatedAreaOptimitionController::run  " << this << std::endl;

  //  auto numPointsRecorded = double( pointsPointer->size() );

  if( pointsPointer->size() > 2 ) {
    auto collectionPointer = new ThreadWeaver::Collection();

    auto optimizeXYJob = ThreadWeaver::make_job( [=] {
      std::cout << "optimizeXYJob " << this << std::endl;

      PS::Squared_distance_cost cost;

      CT_3_2xy      ct;
      Polygon_3_2xy polygon( pointsPointer->cbegin(), pointsPointer->cend() );
      ct.insert_constraint( polygon );

      PS::simplify( ct, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

      polyline_XY = std::make_shared< std::vector< Point_3 > >();

      for( auto cit = ct.constraints_begin(), citend = ct.constraints_end(); cit != citend; ++cit ) {
        for( auto vit = ct.points_in_constraint_begin( *cit ), vitend = ct.points_in_constraint_end( *cit ); vit != vitend; ++vit ) {
          polyline_XY->push_back( *vit );
        }
      }

      if( polyline_XY->size() > 1 ) {
        for( auto p1 = polyline_XY->cbegin(), p2 = polyline_XY->cbegin() + 1, end = polyline_XY->cend(); p2 != end; ++p1, ++p2 ) {
          minDistanceSquaredXY = std::min( minDistanceSquaredXY, CGAL::squared_distance( *p1, *p2 ) );
        }
      }

      // correct scaling: /4 on the squared value equals /2 on the not squared
      minDistanceSquaredXY /= 4;
    } );
    *collectionPointer << optimizeXYJob;

    auto optimizeXZJob = ThreadWeaver::make_job( [=] {
      std::cout << "optimizeXZJob " << this << std::endl;

      PS::Squared_distance_cost cost;

      CT_3_2xz      ct;
      Polygon_3_2xz polygon( pointsPointer->cbegin(), pointsPointer->cend() );
      ct.insert_constraint( polygon );

      PS::simplify( ct, cost, PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ) );

      polyline_XZ = std::make_shared< std::vector< Point_3 > >();

      for( auto cit = ct.constraints_begin(), citend = ct.constraints_end(); cit != citend; ++cit ) {
        for( auto vit = ct.points_in_constraint_begin( *cit ), vitend = ct.points_in_constraint_end( *cit ); vit != vitend; ++vit ) {
          polyline_XZ->push_back( *vit );
        }
      }

      if( polyline_XZ->size() > 1 ) {
        for( auto p1 = polyline_XZ->cbegin(), p2 = polyline_XZ->cbegin() + 1, end = polyline_XZ->cend(); p2 != end; ++p1, ++p2 ) {
          minDistanceSquaredXZ = std::min( minDistanceSquaredXZ, CGAL::squared_distance( *p1, *p2 ) );
        }
      }

      // correct scaling: /4 on the squared value equals /2 on the not squared
      minDistanceSquaredXZ /= 4;
    } );
    *collectionPointer << optimizeXZJob;

    auto unionJob = ThreadWeaver::make_job( [=] {
      std::cout << "unionJob " << this << std::endl;

      std::cout << "polyline_XY " << polyline_XY->size() << std::endl;
      //    for(const auto& p : *polyline_XY){
      //      std::cout <<p<<std::endl;
      //    }
      std::cout << "polyline_XZ " << polyline_XZ->size() << std::endl;
      //    for(const auto& p : *polyline_XZ){
      //      std::cout <<p<<std::endl;
      //    }

      //      std::cout << "pointsUnion " << pointsUnion.size() << std::endl;
      //    for(const auto& p : pointsUnion){
      //      std::cout <<p<<std::endl;
      //    }

      auto n1 = normalsPointer->cbegin();

      resultPoints  = std::make_shared< std::vector< Point_3 > >();
      resultNormals = std::make_shared< std::vector< QVector3D > >();

      int    countP1            = 0;
      int    countP2            = 0;
      double minDistanceSquared = std::min( minDistanceSquaredXY, minDistanceSquaredXZ );
      std::cout << "minDistanceSquared " << minDistanceSquared << std::endl;

      // add the points and their normals to the result vectors, in the order they are in
      // pointsPointer
      for( const auto& p1 : *pointsPointer ) {
        bool alreadyAdded = false;
        for( const auto& p2 : *polyline_XY ) {
          if( CGAL::squared_distance( p1, p2 ) < minDistanceSquared ) {
            resultPoints->push_back( p1 );
            resultNormals->push_back( *n1 );
            alreadyAdded = true;
            break;
          }
        }
        if( !alreadyAdded ) {
          for( const auto& p2 : *polyline_XZ ) {
            if( CGAL::squared_distance( p1, p2 ) < minDistanceSquared ) {
              resultPoints->push_back( p1 );
              resultNormals->push_back( *n1 );
              break;
            }
          }
        }

        ++n1;
        ++countP1;
      }

      std::cout << "pointsPointer " << pointsPointer->size() << std::endl;
      std::cout << "resultPoints " << resultPoints->size() << std::endl;
      //    for(const auto& p : *resultPoints){
      //      std::cout <<p<<std::endl;
      //    }

      Q_EMIT simplifyPolylineResult( resultPoints, resultNormals, maxDeviation );
    } );
    *collectionPointer << unionJob;

    ThreadWeaver::DependencyPolicy::instance().addDependency( ThreadWeaver::Dependency( unionJob, optimizeXYJob ) );
    ThreadWeaver::DependencyPolicy::instance().addDependency( ThreadWeaver::Dependency( unionJob, optimizeXZJob ) );

    collection = ThreadWeaver::make_job( collectionPointer );

    ThreadWeaver::stream() << collection;

  } else {
    Q_EMIT simplifyPolylineResult( pointsPointer, normalsPointer, maxDeviation );
  }

  std::cout << "CultivatedAreaOptimitionController::run2 " << this << std::endl;
}

void
CultivatedAreaOptimitionController::stop() {
  auto colPointer = collection.dynamicCast< ThreadWeaver::Collection >();

  if( colPointer ) {
    colPointer->stop( nullptr );
  }
}

CultivatedAreaOptimitionController::~CultivatedAreaOptimitionController() {
  auto colPointer = collection.dynamicCast< ThreadWeaver::Collection >();

  if( colPointer ) {
    colPointer->stop( nullptr );
  }

  std::cout << "CultivatedAreaOptimitionController::~~~CultivatedAreaOptimitionController " << this << std::endl;
}
