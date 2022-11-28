// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "PathPrimitive.h"

class PathPrimitiveRay : public PathPrimitive {
public:
  PathPrimitiveRay() = default;
  PathPrimitiveRay( const Ray_2& ray, const bool reverse, const double implementWidth, const bool anyDirection, const int32_t passNumber );

  virtual Type getType() override { return Type::Ray; }

public:
  bool operator==( PathPrimitiveRay& b );
  bool operator==( const PathPrimitiveRay& b ) const;

public:
  virtual double distanceToPointSquared( const Point_2 point ) override;
  virtual bool   isOn( const Point_2 ) override;
  virtual bool   leftOf( const Point_2 point ) override;
  virtual double angleAtPointDegrees( const Point_2 ) override;

  virtual bool    intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) override;
  virtual Line_2  perpendicularAtPoint( const Point_2 point ) override;
  virtual Point_2 orthogonalProjection( const Point_2 point ) override;
  virtual Line_2& supportingLine( const Point_2 point ) override;

  virtual void setSource( const Point_2 point ) override;
  virtual void setTarget( const Point_2 point ) override;
  virtual void transform( const Aff_transformation_2& transformation ) override;

  virtual std::shared_ptr< PathPrimitive > createReverse() override;
  virtual std::shared_ptr< PathPrimitive > createNextPrimitive( const bool left ) override;

  virtual void print() override;

public:
  double angleLineDegrees = 0;
  Ray_2  ray;
  Line_2 supportLine;

  bool reverse = false;
};
