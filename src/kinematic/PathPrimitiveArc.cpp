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

#include "PathPrimitiveArc.h"
#include "helpers/cgalHelper.h"

inline void
PathPrimitiveArc::setHelpers() {
  {
    auto bisector1 = CGAL::bisector( startPoint, middlePoint );
    auto bisector2 = CGAL::bisector( middlePoint, endPoint );
    auto result    = CGAL::intersection( bisector1, bisector2 );
    centerPoint    = *boost::get< Point_2 >( &*result );
  }

  startToCenterLine = Line_2( startPoint, centerPoint );
  endToCenterLine   = Line_2( endPoint, centerPoint );
  radius            = std::sqrt( CGAL::squared_distance( startPoint, centerPoint ) );
  radiusSquared     = radius * radius;
  curvatureValue    = 1. / radius;
  bisectorLine      = CGAL::bisector( startToCenterLine, endToCenterLine );

  {
    EpickCircularConverter epickToCircular;

    auto startPointCircular  = epickToCircular( startPoint );
    auto middlePointCircular = epickToCircular( middlePoint );
    auto endPointCircular    = epickToCircular( endPoint );

    if( CGAL::orientation( startPointCircular, middlePointCircular, endPointCircular ) != CGAL::COUNTERCLOCKWISE ) {
      std::swap( startPointCircular, endPointCircular );
      arcClockWise = true;
    } else {
      arcClockWise = false;
    }

    arc = CircularArc_2( startPointCircular, middlePointCircular, endPointCircular );
  }
}

PathPrimitiveArc::PathPrimitiveArc( const Point_2& startPoint,
                                    const Point_2& middlePoint,
                                    const Point_2& endPoint,
                                    double         implementWidth,
                                    bool           anyDirection,
                                    int32_t        passNumber )
    : PathPrimitive( anyDirection, implementWidth, passNumber )
    , startPoint( startPoint )
    , middlePoint( middlePoint )
    , endPoint( endPoint ) {
  setHelpers();
}

bool
PathPrimitiveArc::operator==( PathPrimitiveArc& b ) const {
  return startPoint == b.startPoint && endPoint == b.endPoint && centerPoint == b.centerPoint;
}

bool
PathPrimitiveArc::operator==( const PathPrimitiveArc& b ) const {
  return startPoint == b.startPoint && endPoint == b.endPoint && centerPoint == b.centerPoint;
}

std::shared_ptr< PathPrimitive >
PathPrimitiveArc::createReverse() {
  return std::make_shared< PathPrimitiveArc >( endPoint, middlePoint, startPoint, implementWidth, anyDirection, passNumber );
}

std::shared_ptr< PathPrimitive >
PathPrimitiveArc::createNextPrimitive( bool left ) {
  auto offsetVector = polarOffsetRad( angleOfLineRadians( bisectorLine ) + M_PI, left ? implementWidth : -implementWidth );

  return std::make_shared< PathPrimitiveArc >( startPoint - offsetVector,
                                               middlePoint - offsetVector,
                                               endPoint - offsetVector,
                                               implementWidth,
                                               anyDirection,
                                               passNumber + ( left ? 1 : -1 ) );
}

void
PathPrimitiveArc::print() {
  std::cout << "PathPrimitiveArc: start: " << startPoint << ", middle: " << middlePoint << ", end: " << endPoint
            << ", center: " << centerPoint << ", radius: " << radius << ", curvature: " << curvatureValue << ", clockwise: " << arcClockWise
            << std::endl;
}

double
PathPrimitiveArc::distanceToPointSquared( const Point_2 point ) {
  auto leftOfStartToCenterLine = startToCenterLine.has_on_negative_side( point ) ^ arcClockWise;
  auto rightOfEndToCenterLine  = endToCenterLine.has_on_positive_side( point ) ^ arcClockWise;

  if( leftOfStartToCenterLine && rightOfEndToCenterLine ) {
    return std::pow( std::sqrt( CGAL::squared_distance( point, centerPoint ) ) - radius, 2 );
  }

  if( !leftOfStartToCenterLine && rightOfEndToCenterLine ) {
    return CGAL::squared_distance( point, startPoint );
  }

  if( leftOfStartToCenterLine && !rightOfEndToCenterLine ) {
    return CGAL::squared_distance( point, endPoint );
  }

  return qInf();

  //  if( bisectorLine.has_on_positive_side( point ) ) {
  //    return CGAL::squared_distance( point, startPoint );
  //  }

  //  return CGAL::squared_distance( point, endPoint );
}

bool
PathPrimitiveArc::isOn( const Point_2 point ) {
  auto leftOfStartToCenterLine = startToCenterLine.has_on_negative_side( point ) ^ arcClockWise;
  auto rightOfEndToCenterLine  = endToCenterLine.has_on_positive_side( point ) ^ arcClockWise;

  return leftOfStartToCenterLine && rightOfEndToCenterLine;
}

bool
PathPrimitiveArc::leftOf( const Point_2 point ) {
  auto distance = CGAL::squared_distance( point, centerPoint ) - radiusSquared;
  return !std::signbit( distance ) ^ arcClockWise;
}

double
PathPrimitiveArc::angleAtPointDegrees( const Point_2 point ) {
  auto line  = Line_2( point, centerPoint );
  auto angle = angleOfLineDegrees( line );

  return normalizeAngleDegrees( angle + 90 + ( !arcClockWise ? 180 : 0 ) );
}

double
PathPrimitiveArc::curvature() {
  return curvatureValue;
}

bool
PathPrimitiveArc::intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) {
  EpickCircularConverter epickToCircular;
  CircularEpickConverter circularToEpick;

  auto line = epickToCircular( lineToIntersect );

  typedef typename CGAL::CK2_Intersection_traits< Circular_k, CircularArc_2, CircularLine_2 >::type Intersection_result;
  std::vector< Intersection_result >                                                                results;

  CGAL::intersection( arc, line, std::back_inserter( results ) );

  if( !results.empty() ) {
    auto point = boost::get< std::pair< CircularArcPoint_2, unsigned int > >( ( results.front() ) );

    resultingPoint = Point_2( CGAL::to_double( point.first.x() ), CGAL::to_double( point.first.y() ) );

    return true;
  }

  return false;
}

Line_2
PathPrimitiveArc::perpendicularAtPoint( const Point_2 point ) {
  return Line_2( centerPoint, point );
}

Point_2
PathPrimitiveArc::orthogonalProjection( const Point_2 point ) {
  Point_2 resultingPoint;
  intersectWithLine( perpendicularAtPoint( point ), resultingPoint );
  return resultingPoint;
}

Line_2&
PathPrimitiveArc::supportingLine( const Point_2 point ) {
  Point_2 resultingPoint;

  auto line = perpendicularAtPoint( point );

  intersectWithLine( line, resultingPoint );

  resultingLine = line.perpendicular( resultingPoint );

  return resultingLine;
}

void
PathPrimitiveArc::transform( const Aff_transformation_2& transformation ) {
  startPoint  = startPoint.transform( transformation );
  middlePoint = middlePoint.transform( transformation );
  endPoint    = endPoint.transform( transformation );

  setHelpers();
}
