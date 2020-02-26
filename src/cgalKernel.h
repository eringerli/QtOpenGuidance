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

#ifndef CGALKERNEL_H
#define CGALKERNEL_H

//#ifndef __clang_analyzer__

// standard includes
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdint.h>

#include <QVector3D>

// choose the kernel
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef K::Point_2                                          Point_2;
typedef K::Point_3                                          Point_3;
typedef K::Segment_2                                        Segment_2;
typedef K::Segment_3                                        Segment_3;
typedef K::Vector_2                                         Vector_2;
typedef K::Vector_3                                         Vector_3;
typedef K::Line_2                                           Line_2;
typedef K::Line_3                                           Line_3;
typedef K::Circle_2                                         Circle_2;
typedef K::Iso_rectangle_2                                  Iso_rectangle_2;

#include <CGAL/Aff_transformation_2.h>
typedef CGAL::Aff_transformation_2<K>                       Transformation_2;

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
typedef CGAL::Polygon_2<K>                                  Polygon_2;
typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;

// basic conversions
inline const QVector3D convertPoint3ToQVector3D( const Point_3& point ) {
  return QVector3D( float( point.x() ), float( point.y() ), float( point.z() ) );
}

inline const QVector3D convertPoint2ToQVector3D( Point_2 point ) {
  return QVector3D( float( point.x() ), float( point.y() ), 0 );
}

inline const Point_3 convertQVector3DToPoint3( QVector3D point ) {
  return Point_3( point.x(), point.y(), point.z() );
}

inline const Segment_2 to2D( const Segment_3& segment ) {
  return Segment_2( Point_2( segment.source().x(), segment.source().y() ),
                    Point_2( segment.target().x(), segment.target().y() ) );
}

inline const Point_2 to2D( const Point_3& point ) {
  return Point_2( point.x(), point.y() );
}

inline Vector_2 polarOffset( double angle, double distance ) {
  return Vector_2( std::sin( angle ) * distance, -std::cos( angle ) * distance );
}
//#endif // not __clang_analyzer__

#endif // CGALKERNEL_H
