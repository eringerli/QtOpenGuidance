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

#include "PathPrimitive.h"
#include "PathPrimitiveLine.h"
#include "PathPrimitiveRay.h"
#include "PathPrimitiveSegment.h"
#include "PathPrimitiveSequence.h"

#include <QtMath>

PathPrimitive::PathPrimitive( const bool anyDirection, const double implementWidth, const int32_t passNumber )
  : anyDirection( anyDirection ), implementWidth( implementWidth ), passNumber( passNumber ) {}

const PathPrimitiveLine* PathPrimitive::castToLine() {
  if( getType() == Type::Line ) {
    return static_cast<PathPrimitiveLine*>( this );
  }

  return nullptr;

}

const PathPrimitiveRay* PathPrimitive::castToRay() {
  if( getType() == Type::Ray ) {
    return static_cast<PathPrimitiveRay*>( this );
  }

  return nullptr;

}

const PathPrimitiveSegment* PathPrimitive::castToSegment() {
  if( getType() == Type::Segment ) {
    return static_cast<PathPrimitiveSegment*>( this );
  }

  return nullptr;

}

const PathPrimitiveSequence* PathPrimitive::castToSequence() {
  if( getType() == Type::Sequence ) {
    return static_cast<PathPrimitiveSequence*>( this );
  }

  return nullptr;

}

void PathPrimitive::setSource( const Point_2 ) {}

void PathPrimitive::setTarget( const Point_2 ) {}

std::shared_ptr<PathPrimitive> PathPrimitive::createReverse() {
  return nullptr;
}

std::shared_ptr<PathPrimitive> PathPrimitive::createNextPrimitive( const bool ) {
  return nullptr;
}

void PathPrimitive::print() {
  std::cout << "PathPrimitive" << std::endl;
}

double PathPrimitive::offsetSign( const Point_2 point ) {
  return leftOf( point ) ? -1 : 1;
}

double PathPrimitive::distanceToPoint( const Point_2 point ) {
  return std::sqrt( distanceToPointSquared( point ) );
}
