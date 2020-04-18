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

#include "PlanGlobal.h"

PlanGlobal::PlanGlobal()
  : Plan() {}

PlanGlobal::PlanGlobal( const Plan::Type type )
  : Plan( type ) {}

void PlanGlobal::resetPlanWith( Plan::PrimitiveSharedPointer referencePrimitive ) {
  plan->clear();
  plan->push_back( referencePrimitive );

  for( std::size_t i = 0; i < pathsInReserve; ++i ) {
    createNewPrimitiveOnTheLeft();
    createNewPrimitiveOnTheRight();
  }
}

void PlanGlobal::createNewPrimitiveOnTheLeft() {
  if( !plan->empty() ) {
    plan->push_front( plan->front()->createNextPrimitive( true, false ) );
  }
}

void PlanGlobal::createNewPrimitiveOnTheRight() {
  if( !plan->empty() ) {
    plan->push_back( plan->back()->createNextPrimitive( false, false ) );
  }
}

void PlanGlobal::expand( const Point_2& position2D ) {
  if( !plan->empty() ) {
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
  }
}
