// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// only "lightweight" CGAL-stuff here!

#pragma once

//#ifndef __clang_analyzer__

// standard includes
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>

#include <QVector3D>
#include <QtMath>

// choose the kernel
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
using Epick = CGAL::Exact_predicates_inexact_constructions_kernel;

using Point_2         = Epick::Point_2;
using Point_3         = Epick::Point_3;
using Ray_2           = Epick::Ray_2;
using Segment_2       = Epick::Segment_2;
using Segment_3       = Epick::Segment_3;
using Vector_2        = Epick::Vector_2;
using Vector_3        = Epick::Vector_3;
using Line_2          = Epick::Line_2;
using Line_3          = Epick::Line_3;
using Circle_2        = Epick::Circle_2;
using Iso_rectangle_2 = Epick::Iso_rectangle_2;
using Direction_2     = Epick::Direction_2;
using Plane_3         = Epick::Plane_3;

#include <CGAL/Bbox_2.h>
using Bbox_2 = CGAL::Bbox_2;

#include <CGAL/intersections.h>
using Intersect_2 = Epick::Intersect_2;

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
using Polygon_2            = CGAL::Polygon_2< Epick >;
using Polygon_with_holes_2 = CGAL::Polygon_with_holes_2< Epick >;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
using Epeck = CGAL::Exact_predicates_exact_constructions_kernel;

using EpickEpeckConverter = CGAL::Cartesian_converter< Epick, Epeck >;

//#endif // not __clang_analyzer__
