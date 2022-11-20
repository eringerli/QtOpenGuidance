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
#include <limits>

#include <QDebug>

#include <QElapsedTimer>

Plan::Plan() { plan = std::make_shared< std::deque< std::shared_ptr< PathPrimitive > > >(); }

Plan::Plan( const Plan::Type type ) : type( type ) { plan = std::make_shared< std::deque< std::shared_ptr< PathPrimitive > > >(); }

void
Plan::transform( const Aff_transformation_2& transformation ) {
  for( const auto& it : *plan ) {
    it->transform( transformation );
  }
}

Plan::ConstPrimitiveIterator
Plan::getNearestPrimitive( const Point_2& position2D, double& distanceSquared ) {
  distanceSquared = std::numeric_limits< double >::infinity();

  if( plan->empty() ) {
    return plan->cend();
  }

  //  QElapsedTimer timer;
  //  timer.start();

  auto nearestPrimitiveOnTheLeft = std::partition_point(
    plan->cbegin(), plan->cend(), [&position2D]( auto const& primitive ) { return !( primitive->leftOf( position2D ) ); } );

  if( nearestPrimitiveOnTheLeft == plan->cend() ) {
    --nearestPrimitiveOnTheLeft;
  }

  distanceSquared = ( *nearestPrimitiveOnTheLeft )->distanceToPointSquared( position2D );

  if( distanceSquared > ( *nearestPrimitiveOnTheLeft )->implementWidth ) {
    if( nearestPrimitiveOnTheLeft != plan->cbegin() ) {
      --nearestPrimitiveOnTheLeft;
      distanceSquared = ( *nearestPrimitiveOnTheLeft )->distanceToPointSquared( position2D );
    }
  }

  //  qDebug() << "Cycle Time Plan::getNearestPrimitive:" << timer.nsecsElapsed() << "ns";

  return nearestPrimitiveOnTheLeft;
}
