// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "PathPrimitive.h"

class PathPrimitiveSequence : public PathPrimitive {
public:
  PathPrimitiveSequence() = default;
  PathPrimitiveSequence( const std::vector< Point_2 >& polyline,
                         const double                  implementWidth,
                         const bool                    anyDirection,
                         const int32_t                 passNumber );
  PathPrimitiveSequence( const std::vector< std::shared_ptr< PathPrimitive > >& sequence,
                         const std::vector< Line_2 >&                           bisectors,
                         const double                                           implementWidth,
                         const bool                                             anyDirection,
                         const int32_t                                          passNumber );

  virtual Type getType() override { return Type::Sequence; }

  void updateWithPolyline( const std::vector< Point_2 >& polyline );

public:
  virtual double distanceToPointSquared( const Point_2 point ) override;
  virtual bool   isOn( const Point_2 ) override;
  virtual bool   leftOf( const Point_2 point ) override;
  virtual double angleAtPointDegrees( const Point_2 ) override;

  virtual bool    intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) override;
  virtual Line_2  perpendicularAtPoint( const Point_2 point ) override;
  virtual Point_2 orthogonalProjection( const Point_2 point ) override;
  virtual Line_2& supportingLine( const Point_2 point ) override;

  virtual void transform( const Aff_transformation_2& transformation ) override;

  virtual std::shared_ptr< PathPrimitive > createReverse() override;
  virtual std::shared_ptr< PathPrimitive > createNextPrimitive( bool left ) override;

  virtual void print() override;

  const size_t                            findSequencePrimitiveIndex( const Point_2 point ) const;
  const std::shared_ptr< PathPrimitive >& findSequencePrimitive( const Point_2 point ) const;

public:
  std::vector< Point_2 >                          polyline;
  std::vector< std::shared_ptr< PathPrimitive > > sequence;
  Line_2                                          supportLine;

  std::vector< Line_2 > bisectors;
};
