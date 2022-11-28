// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "helpers/cgalHelper.h"

#include <CGAL/Aff_transformation_2.h>
#include <CGAL/aff_transformation_tags.h>
typedef CGAL::Aff_transformation_2< Epick > Aff_transformation_2;

class PathPrimitiveArc;
class PathPrimitiveLine;
class PathPrimitiveRay;
class PathPrimitiveSegment;
class PathPrimitiveSequence;

class PathPrimitive {
public:
  PathPrimitive() = default;
  PathPrimitive( const bool anyDirection, const double implementWidth, const int32_t passNumber );

  enum class Type : uint8_t { Base = 0, Arc, Line, Ray, Segment, Sequence };

  virtual Type getType() { return Type::Base; }

  const PathPrimitiveArc*      castToArc();
  const PathPrimitiveLine*     castToLine();
  const PathPrimitiveRay*      castToRay();
  const PathPrimitiveSegment*  castToSegment();
  const PathPrimitiveSequence* castToSequence();

public:
  virtual double distanceToPointSquared( const Point_2 point ) = 0;
  virtual bool   isOn( const Point_2 point )                   = 0;
  virtual bool   leftOf( const Point_2 point )                 = 0;
  virtual double angleAtPointDegrees( const Point_2 point )    = 0;
  virtual double curvature();

  virtual bool    intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) = 0;
  virtual Line_2  perpendicularAtPoint( const Point_2 point )                                 = 0;
  virtual Point_2 orthogonalProjection( const Point_2 point )                                 = 0;
  virtual Line_2& supportingLine( const Point_2 point )                                       = 0;

  virtual void setSource( const Point_2 point );
  virtual void setTarget( const Point_2 point );
  virtual void transform( const Aff_transformation_2& transformation ) = 0;

  virtual std::shared_ptr< PathPrimitive > createReverse();
  virtual std::shared_ptr< PathPrimitive > createNextPrimitive( const bool /*left*/ );

  virtual void print();

  double offsetSign( const Point_2 point );

  double distanceToPoint( const Point_2 point );

  static void orderBisectors( std::vector< Line_2 >& bisectorsToOrder, const std::vector< std::shared_ptr< PathPrimitive > >& primitives );
  static void createBisectors( std::back_insert_iterator< std::vector< Line_2 > >     bisectorsOutputIterator,
                               const std::vector< std::shared_ptr< PathPrimitive > >& primitives );

public:
  bool    anyDirection   = false;
  double  implementWidth = 0;
  int32_t passNumber     = 0;
};
