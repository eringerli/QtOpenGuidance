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

#include "PathPrimitiveLine.h"

PathPrimitiveLine::PathPrimitiveLine( const Line_2& line, double implementWidth, bool anyDirection, int32_t passNumber )
  : PathPrimitive( anyDirection, implementWidth, passNumber ), line( line ) {
  angleLineDegrees = angleOfLineDegrees( line );
}

std::shared_ptr<PathPrimitive> PathPrimitiveLine::createReverse() {
  return std::make_shared<PathPrimitiveLine> (
           line.opposite(),
           implementWidth, anyDirection, passNumber );
}

double PathPrimitiveLine::distanceToPointSquared( const Point_2& point ) {
  return CGAL::squared_distance( orthogonalProjection( point ), point );
}

bool PathPrimitiveLine::isOn( const Point_2& ) {
  return true;
}

bool PathPrimitiveLine::leftOf( const Point_2& point ) {
  return line.has_on_negative_side( point );
}

double PathPrimitiveLine::angleAtPointDegrees( const Point_2& ) {
  return angleLineDegrees;
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

Line_2 PathPrimitiveLine::perpendicularAtPoint( const Point_2 point ) {
  return line.perpendicular( point );
}

Point_2 PathPrimitiveLine::orthogonalProjection( const Point_2 point ) {
  return line.projection( point );
}

void PathPrimitiveLine::transform( const Aff_transformation_2& transformation ) {
  line = line.transform( transformation );
}

std::shared_ptr<PathPrimitive> PathPrimitiveLine::createNextPrimitive( bool left, bool reverse ) {
  auto offsetVector = polarOffsetRad( qDegreesToRadians( angleLineDegrees ) + M_PI, left ? implementWidth : -implementWidth );

  if( reverse ) {
    return std::make_shared<PathPrimitiveLine> (
             Line_2( line.point( 0 ) - offsetVector, line.point( 1 ) - offsetVector ).opposite(),
             implementWidth, anyDirection, passNumber + ( left ? 1 : -1 ) );
  } else {
    return std::make_shared<PathPrimitiveLine> (
             Line_2( line.point( 0 ) - offsetVector, line.point( 1 ) - offsetVector ),
             implementWidth, anyDirection, passNumber + ( left ? 1 : -1 ) );
  }
}
