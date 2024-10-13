// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "PathPrimitive.h"

#include <CGAL/Exact_circular_kernel_2.h>
using Circular_k             = CGAL::Exact_circular_kernel_2;
using CircularPoint_2        = Circular_k::Point_2;
using CircularLine_2         = Circular_k::Line_2;
using CircularArc_2          = Circular_k::Circular_arc_2;
using CircularArcPoint_2     = Circular_k::Circular_arc_point_2;
using CircularEpickConverter = CGAL::Cartesian_converter< Circular_k, Epick >;
using EpickCircularConverter = CGAL::Cartesian_converter< Epick, Circular_k >;

class PathPrimitiveCircle : public PathPrimitive {
public:
  PathPrimitiveCircle() = default;
  PathPrimitiveCircle( const Point_2& startPoint,
                       const Point_2& middlePoint,
                       const Point_2& endPoint,
                       const double   implementWidth,
                       const bool     anyDirection,
                       const int32_t  passNumber );

  virtual Type getType() override { return Type::Arc; }

public:
  bool operator==( PathPrimitiveCircle& b ) const;
  bool operator==( const PathPrimitiveCircle& b ) const;

public:
  virtual double distanceToPointSquared( const Point_2 point ) override;
  virtual bool   isOn( const Point_2 ) override;
  virtual bool   leftOf( const Point_2 point ) override;
  virtual double angleAtPointDegrees( const Point_2 ) override;
  virtual double curvature() override;

  virtual bool    intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) override;
  virtual Line_2  perpendicularAtPoint( const Point_2 point ) override;
  virtual Point_2 orthogonalProjection( const Point_2 point ) override;
  virtual Line_2& supportingLine( const Point_2 point ) override;

  virtual void transform( const Aff_transformation_2& transformation ) override;

  virtual std::shared_ptr< PathPrimitive > createReverse() override;
  virtual std::shared_ptr< PathPrimitive > createNextPrimitive( const bool left ) override;

  virtual void print() override;

public:
  Point_2       startPoint;
  Point_2       middlePoint;
  Point_2       endPoint;
  Point_2       centerPoint;
  CircularArc_2 arc;
  bool          arcClockWise = true;
  Line_2        startToCenterLine;
  Line_2        endToCenterLine;
  Line_2        bisectorLine;
  double        radius         = 0;
  double        curvatureValue = 0;
  double        radiusSquared  = 0;

private:
  inline void setHelpers();

  Line_2 resultingLine;
};
