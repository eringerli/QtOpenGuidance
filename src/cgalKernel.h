// Copyright( C ) 2019 Christian Riggenbach
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

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
typedef CGAL::Polygon_2<K>                                  Polygon_2;
typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;

// basic conversions

inline QVector3D convertPoint3ToQVector3D( Point_3 point ) {
  return QVector3D( float( point.x() ), float( point.y() ), float( point.z() ) );
}

inline QVector3D convertPoint2ToQVector3D( Point_2 point ) {
  return QVector3D( float( point.x() ), float( point.y() ), 0 );
}

inline Point_3 convertQVector3DToPoint3( QVector3D point ) {
  return Point_3( point.x(), point.y(), point.z() );
}

#endif // CGALKERNEL_H
