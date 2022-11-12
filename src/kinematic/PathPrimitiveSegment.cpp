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

#include "PathPrimitiveSegment.h"

PathPrimitiveSegment::PathPrimitiveSegment( const Segment_2& segment, double implementWidth, bool anyDirection, int32_t passNumber )
    : PathPrimitive( anyDirection, implementWidth, passNumber ), segment( segment ), supportLine( segment.supporting_line() ) {
  angleLineDegrees = angleOfLineDegrees( supportLine );
}

bool
PathPrimitiveSegment::operator==( PathPrimitiveSegment& b ) {
  return segment == b.segment;
}

bool
PathPrimitiveSegment::operator==( const PathPrimitiveSegment& b ) const {
  return segment == b.segment;
}

std::shared_ptr< PathPrimitive >
PathPrimitiveSegment::createReverse() {
  return std::make_shared< PathPrimitiveSegment >( segment.opposite(), implementWidth, anyDirection, passNumber );
}

std::shared_ptr< PathPrimitive >
PathPrimitiveSegment::createNextPrimitive( bool left ) {
  auto offsetVector = polarOffsetRad( degreesToRadians( angleLineDegrees ) + M_PI, left ? implementWidth : -implementWidth );

  return std::make_shared< PathPrimitiveSegment >( Segment_2( segment.source() - offsetVector, segment.target() - offsetVector ),
                                                   implementWidth,
                                                   anyDirection,
                                                   passNumber + ( left ? 1 : -1 ) );
}

void
PathPrimitiveSegment::print() {
  std::cout << "PathPrimitiveSegment: " << segment << std::endl;
}

double
PathPrimitiveSegment::distanceToPointSquared( const Point_2 point ) {
  Point_2 ortogonalProjection = orthogonalProjection( point );
  double  distance            = CGAL::squared_distance( ortogonalProjection, point );

  if( segment.collinear_has_on( ortogonalProjection ) ) {
    return distance;
  }

  double distToSource = CGAL::squared_distance( point, segment.source() );
  double distToTarget = CGAL::squared_distance( point, segment.target() );

  if( distToSource < distToTarget ) {
    return distToSource;
  } else {
    return distToTarget;
  }
}

bool
PathPrimitiveSegment::isOn( const Point_2 point ) {
  Point_2 ortogonalProjection = supportLine.projection( point );

  return segment.collinear_has_on( ortogonalProjection );
}

bool
PathPrimitiveSegment::leftOf( const Point_2 point ) {
  return supportLine.has_on_negative_side( point );
}

double
PathPrimitiveSegment::angleAtPointDegrees( const Point_2 ) {
  return angleLineDegrees;
}

bool
PathPrimitiveSegment::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  auto result = CGAL::intersection( lineToIntersect, segment );

  if( result ) {
    if( const Point_2* point = boost::get< Point_2 >( &*result ) ) {
      resultingPoint = *point;
      return true;
    }
  }

  return false;
}

Line_2
PathPrimitiveSegment::perpendicularAtPoint( const Point_2 point ) {
  return supportLine.perpendicular( point );
}

Point_2
PathPrimitiveSegment::orthogonalProjection( const Point_2 point ) {
  return supportLine.projection( point );
}

Line_2&
PathPrimitiveSegment::supportingLine( const Point_2 ) {
  return supportLine;
}

void
PathPrimitiveSegment::setSource( const Point_2 point ) {
  segment          = Segment_2( point, segment.target() );
  supportLine      = segment.supporting_line();
  angleLineDegrees = angleOfLineDegrees( supportLine );
}

void
PathPrimitiveSegment::setTarget( const Point_2 point ) {
  segment          = Segment_2( segment.source(), point );
  supportLine      = segment.supporting_line();
  angleLineDegrees = angleOfLineDegrees( supportLine );
}

void
PathPrimitiveSegment::transform( const Aff_transformation_2& transformation ) {
  segment          = segment.transform( transformation );
  supportLine      = segment.supporting_line();
  angleLineDegrees = angleOfLineDegrees( supportLine );
}
