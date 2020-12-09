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
typedef CGAL::Exact_predicates_inexact_constructions_kernel     Epick;

typedef Epick::Point_2                                          Point_2;
typedef Epick::Point_3                                          Point_3;
typedef Epick::Ray_2                                            Ray_2;
typedef Epick::Segment_2                                        Segment_2;
typedef Epick::Segment_3                                        Segment_3;
typedef Epick::Vector_2                                         Vector_2;
typedef Epick::Vector_3                                         Vector_3;
typedef Epick::Line_2                                           Line_2;
typedef Epick::Line_3                                           Line_3;
typedef Epick::Circle_2                                         Circle_2;
typedef Epick::Iso_rectangle_2                                  Iso_rectangle_2;
typedef Epick::Direction_2                                      Direction_2;
typedef Epick::Plane_3                                          Plane_3;

#include <CGAL/Bbox_2.h>
typedef CGAL::Bbox_2                                            Bbox_2;

#include <CGAL/intersections.h>
typedef Epick::Intersect_2 Intersect_2;

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
typedef CGAL::Polygon_2<Epick>                                  Polygon_2;
typedef CGAL::Polygon_with_holes_2<Epick>                       Polygon_with_holes_2;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Exact_predicates_exact_constructions_kernel       Epeck;

typedef CGAL::Cartesian_converter<Epick, Epeck>                 EpickEpeckConverter;

//#endif // not __clang_analyzer__
