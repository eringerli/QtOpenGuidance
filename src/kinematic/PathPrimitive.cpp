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

#include <QObject>

#include <QLineF>
#include <QPointF>
#include <QtMath>

#include "../cgalKernel.h"

#include <QDebug>


const PathPrimitiveLine* PathPrimitive::castToLine() {
  if( getType() == Type::Line ) {
    return static_cast<PathPrimitiveLine*>( this );
  } else {
    return nullptr;
  }
}
const PathPrimitiveSegment* PathPrimitive::castToSegment() {
  if( getType() == Type::Segment ) {
    return static_cast<PathPrimitiveSegment*>( this );
  } else {
    return nullptr;
  }
}


std::shared_ptr<PathPrimitive> PathPrimitiveLine::createReverse() {
  return std::make_shared<PathPrimitiveLine> (
           line.opposite(),
           implementWidth, anyDirection, passNumber );
}

double PathPrimitiveLine::distanceToPointSquared( const Point_2& point ) {
  Point_2 ortogonalProjection = line.projection( point );

  return CGAL::squared_distance( ortogonalProjection, point );
}

bool PathPrimitiveLine::isOn( const Point_2& point ) {
  return true;
}

bool PathPrimitiveLine::leftOf( const Point_2& point ) {
  return line.has_on_negative_side( point );
}

double PathPrimitiveLine::angleAtPoint( const Point_2& ) {
  return angleOfLineDegrees( line );
}

bool PathPrimitiveLine::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  auto result = CGAL::intersection( line, lineToIntersect );

  if( result ) {
    if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
      resultingPoint = *point;
      return true;
    }
  }

  return false;
}


std::shared_ptr<PathPrimitive> PathPrimitiveSegment::createReverse() {
  return std::make_shared<PathPrimitiveSegment> (
           segment.opposite(),
           implementWidth, anyDirection, passNumber );
}

double PathPrimitiveSegment::distanceToPointSquared( const Point_2& point ) {
  Point_2 ortogonalProjection = segment.supporting_line().projection( point );
  double distance = CGAL::squared_distance( ortogonalProjection, point );

  if( segment.collinear_has_on( ortogonalProjection ) ) {
    return distance;
  } else {
    double distToSource = CGAL::squared_distance( point, segment.source() );
    double distToTarget = CGAL::squared_distance( point, segment.target() );

    if( distToSource < distToTarget ) {
      return distToSource;
    } else {
      return distToTarget;
    }
  }
}

bool PathPrimitiveSegment::isOn( const Point_2& point ) {
  Point_2 ortogonalProjection = segment.supporting_line().projection( point );

  return segment.collinear_has_on( ortogonalProjection );
}

bool PathPrimitiveSegment::leftOf( const Point_2& point ) {
  return segment.supporting_line().has_on_negative_side( point );
}

double PathPrimitiveSegment::angleAtPoint( const Point_2& ) {
  return angleOfLineDegrees( segment.supporting_line() );
}

bool PathPrimitiveSegment::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  auto result = CGAL::intersection( lineToIntersect, segment );

  if( result ) {
    if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
      resultingPoint = *point;
      return true;
    }
  }

  return false;
}
