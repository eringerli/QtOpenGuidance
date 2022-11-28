// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PathPrimitiveLine.h"

PathPrimitiveLine::PathPrimitiveLine( const Line_2& line, double implementWidth, bool anyDirection, int32_t passNumber )
    : PathPrimitive( anyDirection, implementWidth, passNumber ), line( line ) {
  angleLineDegrees = angleOfLineDegrees( line );
}

bool
PathPrimitiveLine::operator==( PathPrimitiveLine& b ) {
  return passNumber == b.passNumber;
}

bool
PathPrimitiveLine::operator==( const PathPrimitiveLine& b ) const {
  return passNumber == b.passNumber;
}

std::shared_ptr< PathPrimitive >
PathPrimitiveLine::createReverse() {
  return std::make_shared< PathPrimitiveLine >( line.opposite(), implementWidth, anyDirection, passNumber );
}

double
PathPrimitiveLine::distanceToPointSquared( const Point_2 point ) {
  return CGAL::squared_distance( orthogonalProjection( point ), point );
}

bool
PathPrimitiveLine::isOn( const Point_2 ) {
  return true;
}

bool
PathPrimitiveLine::leftOf( const Point_2 point ) {
  return line.has_on_negative_side( point );
}

double
PathPrimitiveLine::angleAtPointDegrees( const Point_2 ) {
  return angleLineDegrees;
}

bool
PathPrimitiveLine::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  auto result = CGAL::intersection( line, lineToIntersect );

  if( result ) {
    if( const Point_2* point = boost::get< Point_2 >( &*result ) ) {
      resultingPoint = *point;
      return true;
    }
  }

  return false;
}

Line_2
PathPrimitiveLine::perpendicularAtPoint( const Point_2 point ) {
  return line.perpendicular( point );
}

Point_2
PathPrimitiveLine::orthogonalProjection( const Point_2 point ) {
  return line.projection( point );
}

Line_2&
PathPrimitiveLine::supportingLine( const Point_2 ) {
  return line;
}

void
PathPrimitiveLine::transform( const Aff_transformation_2& transformation ) {
  line = line.transform( transformation );
}

std::shared_ptr< PathPrimitive >
PathPrimitiveLine::createNextPrimitive( bool left ) {
  auto offsetVector = polarOffsetRad( degreesToRadians( angleLineDegrees ) + M_PI, left ? implementWidth : -implementWidth );

  return std::make_shared< PathPrimitiveLine >( Line_2( line.point( 0 ) - offsetVector, line.point( 1 ) - offsetVector ),
                                                implementWidth,
                                                anyDirection,
                                                passNumber + ( left ? 1 : -1 ) );
}

void
PathPrimitiveLine::print() {
  std::cout << "PathPrimitiveLine: " << line << std::endl;
}
