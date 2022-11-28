// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlanOptimitionController.h"

#include <algorithm>

#include <CGAL/Polyline_simplification_2/simplify.h>
namespace PS = CGAL::Polyline_simplification_2;

PlanOptimitionController::PlanOptimitionController( std::shared_ptr< std::vector< Point_2 > > pointsPointer, const double maxDeviation )
    : pointsPointer( pointsPointer ), maxDeviation( maxDeviation ) {
  std::cout << "PlanOptimitionController::PlanOptimitionController " << this << std::endl;
}

void
PlanOptimitionController::run() {
  std::cout << "PlanOptimitionController::run  " << this << std::endl;

  if( pointsPointer->size() > 2 ) {
    job = ThreadWeaver::make_job( [=] {
      std::cout << "job " << this << std::endl;
      PS::Squared_distance_cost cost;

      auto polylineOut = std::make_shared< std::vector< Point_2 > >();

      PS::simplify( pointsPointer->begin(),
                    pointsPointer->end(),
                    cost,
                    PS::Stop_above_cost_threshold( maxDeviation * maxDeviation ),
                    std::back_inserter( *polylineOut ) );

      Q_EMIT simplifyPolylineResult( polylineOut );
    } );

    ThreadWeaver::stream() << job;

  } else {
    Q_EMIT simplifyPolylineResult( pointsPointer );
  }

  std::cout << "PlanOptimitionController::run2 " << this << std::endl;
}

void
PlanOptimitionController::stop() {
  job->requestAbort();
}

PlanOptimitionController::~PlanOptimitionController() {
  job->requestAbort();

  std::cout << "PlanOptimitionController::~~~PlanOptimitionController " << this << std::endl;
}
