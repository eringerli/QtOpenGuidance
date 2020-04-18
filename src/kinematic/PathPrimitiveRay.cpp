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

#include "PathPrimitiveRay.h"

PathPrimitiveRay::PathPrimitiveRay( const Ray_2& ray, double implementWidth, bool anyDirection, int32_t passNumber )
  : PathPrimitive( anyDirection, implementWidth, passNumber ), ray( ray ), supportingLine( ray.supporting_line() ) {
  angleLineDegrees = angleOfLineDegrees( supportingLine );
}

std::shared_ptr<PathPrimitive> PathPrimitiveRay::createReverse() {
  return std::make_shared<PathPrimitiveRay> (
           ray.opposite(),
           implementWidth, anyDirection, passNumber );
}

double PathPrimitiveRay::distanceToPointSquared( const Point_2& point ) {
  Point_2 ortogonalProjection = orthogonalProjection( point );
  double distance = CGAL::squared_distance( ortogonalProjection, point );

  if( ray.collinear_has_on( ortogonalProjection ) ) {
    return distance;
  } else {
    return CGAL::squared_distance( point, ray.source() );
  }
}

bool PathPrimitiveRay::isOn( const Point_2& point ) {
  Point_2 ortogonalProjection = supportingLine.projection( point );

  return ray.collinear_has_on( ortogonalProjection );
}

bool PathPrimitiveRay::leftOf( const Point_2& point ) {
  return supportingLine.has_on_negative_side( point );
}

double PathPrimitiveRay::angleAtPointDegrees( const Point_2& ) {
  return angleLineDegrees;
}

bool PathPrimitiveRay::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  auto result = CGAL::intersection( lineToIntersect, ray );

  if( result ) {
    if( const Point_2* point = boost::get<Point_2>( &*result ) ) {
      resultingPoint = *point;
      return true;
    }
  }

  return false;
}

Line_2 PathPrimitiveRay::perpendicularAtPoint( const Point_2 point ) {
  return supportingLine.perpendicular( point );
}

Point_2 PathPrimitiveRay::orthogonalProjection( const Point_2 point ) {
  return supportingLine.projection( point );
}

void PathPrimitiveRay::transform( const Aff_transformation_2& transformation ) {
  ray = ray.transform( transformation );
  supportingLine = ray.supporting_line();
  angleLineDegrees = angleOfLineDegrees( supportingLine );
}

std::shared_ptr<PathPrimitive> PathPrimitiveRay::createNextPrimitive( bool left, bool reverse ) {
  auto offsetVector = polarOffsetRad( qDegreesToRadians( angleLineDegrees ) + M_PI, left ? implementWidth : -implementWidth );

  if( reverse ) {
    return std::make_shared<PathPrimitiveRay> (
             Ray_2( ray.source() - offsetVector, ray.opposite().direction() ),
             implementWidth, anyDirection, passNumber + ( left ? 1 : -1 ) );
  } else {
    return std::make_shared<PathPrimitiveRay> (
             Ray_2( ray.source() - offsetVector, ray.direction() ),
             implementWidth, anyDirection, passNumber + ( left ? 1 : -1 ) );
  }
}
