// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "PathPrimitive.h"
#include "helpers/cgalHelper.h"

class Plan {
public:
  Plan();

public:
  typedef std::shared_ptr< PathPrimitive >     PrimitiveSharedPointer;
  typedef std::deque< PrimitiveSharedPointer > PlanData;

  std::shared_ptr< PlanData > plan;

  typedef decltype( plan->begin() )  PrimitiveIterator;
  typedef decltype( plan->cbegin() ) ConstPrimitiveIterator;

public:
  void transform( const Aff_transformation_2& transformation );

  ConstPrimitiveIterator getNearestPrimitive( const Point_2& position2D, double& distanceSquared, ConstPrimitiveIterator* tip = nullptr );
};

Q_DECLARE_METATYPE( Plan );
