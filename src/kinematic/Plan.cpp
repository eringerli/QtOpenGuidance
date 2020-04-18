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

#include "Plan.h"

#include <QElapsedTimer>

Plan::Plan() {
  plan = std::make_shared<std::deque<std::shared_ptr<PathPrimitive>>>();
}

Plan::Plan( const Plan::Type type )
  : type( type ) {
  plan = std::make_shared<std::deque<std::shared_ptr<PathPrimitive>>>();
}

void Plan::transform( const Aff_transformation_2& transformation ) {
  for( auto it = plan->cbegin(), end = plan->cend(); it != end; ++it ) {
    ( *it )->transform( transformation );
  }
}

Plan::ConstPrimitiveIterator Plan::getNearestPrimitive( const Point_2& position2D, double& distanceSquared ) {
//        QElapsedTimer timer;
//        timer.start();
  ConstPrimitiveIterator nearestPrimitive = plan->cend();
  distanceSquared = qInf();

  for( auto it = plan->cbegin(), end = plan->cend(); it != end; ++it ) {
//    QElapsedTimer timer;
//    timer.start();
    double currentDistanceSquared = ( *it )->distanceToPointSquared( position2D );
//    qDebug() << "Cycle Time ( *it )->distanceToPointSquared:" << timer.nsecsElapsed() << "ns";

    if( currentDistanceSquared < distanceSquared ) {
      nearestPrimitive = it;
      distanceSquared = currentDistanceSquared;
    } else {
      if( type == Plan::Type::OnlyLines ) {
        // the plan is ordered, so we can take the fast way out...
        break;
      }
    }
  }

//        qDebug() << "Cycle Time Plan::getNearestPrimitive:" << timer.nsecsElapsed() << "ns";

  return nearestPrimitive;
}
