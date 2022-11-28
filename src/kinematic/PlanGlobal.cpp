// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlanGlobal.h"

#include <QDebug>

#include <QElapsedTimer>

void
PlanGlobal::resetPlanWith( const Plan::PrimitiveSharedPointer& referencePrimitive ) {
  plan->clear();
  plan->push_back( referencePrimitive );

  for( std::size_t i = 0; i < pathsInReserve; ++i ) {
    createNewPrimitiveOnTheLeft();
    createNewPrimitiveOnTheRight();
  }
}

void
PlanGlobal::createNewPrimitiveOnTheLeft() {
  if( !plan->empty() ) {
    plan->push_front( plan->front()->createNextPrimitive( true ) );
  }
}

void
PlanGlobal::createNewPrimitiveOnTheRight() {
  if( !plan->empty() ) {
    plan->push_back( plan->back()->createNextPrimitive( false ) );
  }
}

void
PlanGlobal::expand( Point_2 position2D ) {
  if( !plan->empty() ) {
    //    QElapsedTimer timer;
    //    timer.start();

    // make sure, at least pathsInReserve primitives exist on both sides of the reference
    while( ( plan->size() / 2 ) < pathsInReserve ) {
      createNewPrimitiveOnTheLeft();
      createNewPrimitiveOnTheRight();
    }

    while( ( *( plan->cbegin() + pathsInReserve ) )->leftOf( position2D ) ) {
      createNewPrimitiveOnTheLeft();
    }

    while( !( *( plan->cend() - 1 - pathsInReserve ) )->leftOf( position2D ) ) {
      createNewPrimitiveOnTheRight();
    }

    //    qDebug() << "Cycle Time PlanGlobal::expand:" << timer.nsecsElapsed() << "ns";
  }
}
