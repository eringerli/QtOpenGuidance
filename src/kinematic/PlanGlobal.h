// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Plan.h"

class PlanGlobal : public Plan {
public:
  PlanGlobal() = default;

public:
  void resetPlanWith( const PrimitiveSharedPointer& referencePrimitive );
  void createNewPrimitiveOnTheLeft();
  void createNewPrimitiveOnTheRight();

  void expand( Point_2 position2D );

public:
  std::size_t pathsInReserve = 3;
};

Q_DECLARE_METATYPE( PlanGlobal )
