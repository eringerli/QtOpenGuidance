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

#pragma once

#include "Plan.h"

class PlanGlobal : public Plan {
public:
  PlanGlobal() = default;
  PlanGlobal( const Type type );

public:
  void resetPlanWith( const PrimitiveSharedPointer& referencePrimitive );
  void createNewPrimitiveOnTheLeft();
  void createNewPrimitiveOnTheRight();

  void expand( Point_2 position2D );

public:
  std::size_t pathsInReserve = 3;
};

Q_DECLARE_METATYPE( PlanGlobal )
