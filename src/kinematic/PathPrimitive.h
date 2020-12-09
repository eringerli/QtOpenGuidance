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

#pragma once

#include "../helpers/cgalHelper.h"

#include <CGAL/Aff_transformation_2.h>
#include <CGAL/aff_transformation_tags.h>
typedef CGAL::Aff_transformation_2<Epick>                       Aff_transformation_2;

class PathPrimitiveLine;
class PathPrimitiveRay;
class PathPrimitiveSegment;
class PathPrimitiveSequence;

class PathPrimitive {
  public:
    PathPrimitive() {}

    PathPrimitive( const bool anyDirection, const double implementWidth, const int32_t passNumber )
      : anyDirection( anyDirection ), implementWidth( implementWidth ), passNumber( passNumber ) {}

    virtual ~PathPrimitive() {}

    enum class Type : uint8_t {
      Base = 0,
      Line,
      Ray,
      Segment,
      Sequence
    };

    virtual Type getType() {
      return Type::Base;
    }

    const PathPrimitiveLine* castToLine();
    const PathPrimitiveRay* castToRay();
    const PathPrimitiveSegment* castToSegment();
    const PathPrimitiveSequence* castToSequence();

  public:
    virtual double distanceToPointSquared( const Point_2 point ) = 0;
    virtual bool isOn( const Point_2 point ) = 0;
    virtual bool leftOf( const Point_2 point ) = 0;
    virtual double angleAtPointDegrees( const Point_2 point ) = 0;

    virtual bool intersectWithLine( const Line_2& lineToIntersect, Point_2& resultingPoint ) = 0;
    virtual Line_2 perpendicularAtPoint( const Point_2 point ) = 0;
    virtual Point_2 orthogonalProjection( const Point_2 point ) = 0;
    virtual Line_2& supportingLine( const Point_2 point ) = 0;

    virtual void setSource( const Point_2 point );
    virtual void setTarget( const Point_2 point );
    virtual void transform( const Aff_transformation_2& transformation ) = 0;

    virtual std::shared_ptr<PathPrimitive> createReverse() {
      return nullptr;
    }
    virtual std::shared_ptr<PathPrimitive> createNextPrimitive( const bool /*left*/ ) {
      return nullptr;
    }

    virtual void print() {
      std::cout << "PathPrimitive" << std::endl;
    }

    double offsetSign( const Point_2 point ) {
      return leftOf( point ) ? -1 : 1;
    }

    double distanceToPoint( const Point_2 point ) {
      return std::sqrt( distanceToPointSquared( point ) );
    }


  public:
    bool anyDirection = false;
    double implementWidth = 0;
    int32_t passNumber = 0;
};
