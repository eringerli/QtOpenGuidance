// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Plan.h"
#include <limits>

#include <QDebug>

#include <QElapsedTimer>

Plan::Plan() { plan = std::make_shared< Plan::PlanData >(); }

void
Plan::transform( const Aff_transformation_2& transformation ) {
  for( const auto& it : *plan ) {
    it->transform( transformation );
  }
}

Plan::ConstPrimitiveIterator
Plan::getNearestPrimitive( const Point_2& position2D, double& distanceSquared, ConstPrimitiveIterator* tip ) {
  distanceSquared = std::numeric_limits< double >::infinity();

  if( plan->empty() ) {
    return plan->cend();
  }

  //  QElapsedTimer timer;
  //  timer.start();

  if( tip != nullptr && *tip != plan->cend() ) {
    distanceSquared = ( **tip )->distanceToPointSquared( position2D );

    if( plan->size() == 1 ) {
      return *tip;
    }

    if( distanceSquared < ( ( **tip )->implementWidth / 2. ) ) {
      return *tip;
    }
  }

  auto nearestPrimitiveOnTheLeft = std::partition_point(
    plan->cbegin(), plan->cend(), [&position2D]( auto const& primitive ) { return !( primitive->leftOf( position2D ) ); } );

  if( nearestPrimitiveOnTheLeft == plan->cend() ) {
    --nearestPrimitiveOnTheLeft;
  }

  distanceSquared = ( *nearestPrimitiveOnTheLeft )->distanceToPointSquared( position2D );

  if( distanceSquared > ( ( *nearestPrimitiveOnTheLeft )->implementWidth / 2. ) ) {
    if( nearestPrimitiveOnTheLeft != plan->cbegin() ) {
      --nearestPrimitiveOnTheLeft;
      distanceSquared = ( *nearestPrimitiveOnTheLeft )->distanceToPointSquared( position2D );
    }
  }

  //  std::cout << "Cycle Time Plan::getNearestPrimitive: " << timer.nsecsElapsed() << "ns" << std::endl;

  return nearestPrimitiveOnTheLeft;
}
