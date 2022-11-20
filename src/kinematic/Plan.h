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
