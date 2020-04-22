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

// only "lightweight" CGAL-stuff here!

#pragma once

//#ifndef __clang_analyzer__

// standard includes
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdint.h>

#include <QtMath>
#include <QVector3D>

// choose the kernel
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_2                                          Point_2;
typedef K::Point_3                                          Point_3;
typedef K::Ray_2                                            Ray_2;
typedef K::Segment_2                                        Segment_2;
typedef K::Segment_3                                        Segment_3;
typedef K::Vector_2                                         Vector_2;
typedef K::Vector_3                                         Vector_3;
typedef K::Line_2                                           Line_2;
typedef K::Line_3                                           Line_3;
typedef K::Circle_2                                         Circle_2;
typedef K::Iso_rectangle_2                                  Iso_rectangle_2;

#include <CGAL/Aff_transformation_2.h>
#include <CGAL/aff_transformation_tags.h>
typedef CGAL::Aff_transformation_2<K>                       Aff_transformation_2;

#include <CGAL/Aff_transformation_3.h>
typedef CGAL::Aff_transformation_3<K>                       Aff_transformation_3;

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
typedef CGAL::Polygon_2<K>                                  Polygon_2;
typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;

inline double normalizeAngleRadians( double angle ) {
  while( angle > M_PI ) {
    angle -= M_PI * 2;
  }

  while( angle < -M_PI ) {
    angle += M_PI * 2;
  }

  return angle;
}

inline double normalizeAngleDegrees( double angle ) {
  while( angle > 180 ) {
    angle -= 360;
  }

  while( angle < -180 ) {
    angle += 360;
  }

  return angle;
}

// basic conversions
inline const QVector3D convertPoint3ToQVector3D( const Point_3& point ) {
  return QVector3D( float( point.x() ), float( point.y() ), float( point.z() ) );
}

inline const QVector3D convertPoint2ToQVector3D( const Point_2& point ) {
  return QVector3D( float( point.x() ), float( point.y() ), 0 );
}

inline const Point_3 convertQVector3DToPoint3( const QVector3D& point ) {
  return Point_3( point.x(), point.y(), point.z() );
}

inline const Segment_2 to2D( const Segment_3& segment ) {
  return Segment_2( Point_2( segment.source().x(), segment.source().y() ),
                    Point_2( segment.target().x(), segment.target().y() ) );
}

inline const Point_2 to2D( const Point_3& point ) {
  return Point_2( point.x(), point.y() );
}

inline const Point_3 to3D( const Point_2& point ) {
  return Point_3( point.x(), point.y(), 0 );
}

inline const Segment_3 to3D( const Segment_2& segment ) {
  return Segment_3( Point_3( segment.source().x(), segment.source().y(), 0 ), Point_3( segment.target().x(), segment.target().y(), 0 ) );
}

inline const Vector_3 to3D( const Vector_2& vector ) {
  return Vector_3( vector.x(), vector.y(), 0 );
}

inline Vector_2 polarOffsetRad( const double angleRad, const double distance ) {
  return Vector_2( std::sin( angleRad ) * distance, -std::cos( angleRad ) * distance );
}

inline Vector_2 polarOffsetDegrees( const double angleDegrees, const double distance ) {
  double angleRad = qDegreesToRadians( angleDegrees );
  return Vector_2( std::sin( angleRad ) * distance, -std::cos( angleRad ) * distance );
}

inline double angleOfLineDegrees( const Line_2& line ) {
  Vector_3 north( 1, 0, 0 );
  Vector_2 north2D( 1, 0 );

  Vector_2 lineVector = line.to_vector();

  double angleAB = CGAL::approximate_angle( north, to3D( lineVector ) );
  return normalizeAngleDegrees( ( angleAB ) * CGAL::orientation( north2D, lineVector ) );
}

inline double angleOfLineRadians( const Line_2& line ) {
  return qDegreesToRadians( angleOfLineDegrees( line ) );
}

//#endif // not __clang_analyzer__
