// Copyright( C ) 2021 Christian Riggenbach
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
