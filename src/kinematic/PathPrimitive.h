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

#ifndef PATHPRIMITIVE_H
#define PATHPRIMITIVE_H

#include "../cgalKernel.h"

#include <QDebug>

class PathPrimitiveLine;
class PathPrimitiveSegment;
class PathPrimitiveCircle;


class PathPrimitive {
  public:
    PathPrimitive() {}

    PathPrimitive( bool anyDirection, double width, int passNumber )
      : anyDirection( anyDirection ), width( width ), passNumber( passNumber ) {}

    enum class Type : uint8_t {
      Base = 0,
      Line,
      Segment,
      Circle
    };

    virtual Type getType() {
      return Type::Base;
    }

    const PathPrimitiveLine* castToLine();
    const PathPrimitiveSegment* castToSegment();
//    const PathPrimitiveCircle* castToCircle();

  public:
    virtual double distanceToPoint( const Point_2& point ) = 0;
    virtual void print();

  public:
    bool anyDirection = false;
    double width = 0;
    int passNumber = 0;
};

class PathPrimitiveLine : public PathPrimitive {
  public:
    PathPrimitiveLine()
      : PathPrimitive() {}

    PathPrimitiveLine( const Line_2& line, double width, bool anyDirection, int passNumber )
      : PathPrimitive( anyDirection, width, passNumber ), line( line ) {}

    virtual Type getType() override {
      return Type::Line;
    }

  public:
    bool operator==( PathPrimitiveLine& b ) {
      return line == b.line;
    }
    bool operator==( const PathPrimitiveLine& b )const {
      return line == b.line;
    }

  public:
    void reverse();

  public:
    virtual double distanceToPoint( const Point_2& point ) override;
    virtual void print() override;

  public:
    Line_2 line;
};

class PathPrimitiveSegment : public PathPrimitive {
  public:
    PathPrimitiveSegment()
      : PathPrimitive() {}

    PathPrimitiveSegment( const Segment_2& segment, double width, bool anyDirection, int passNumber )
      : PathPrimitive( anyDirection, width, passNumber ), segment( segment ) {}

    virtual Type getType() override {
      return Type::Segment;
    }

  public:
    bool operator==( PathPrimitiveSegment& b ) {
      return segment == b.segment;
    }
    bool operator==( const PathPrimitiveSegment& b ) const {
      return segment == b.segment;
    }

  public:
    void reverse();

  public:
    virtual double distanceToPoint( const Point_2& point ) override;
    virtual void print() override;

  public:
    Segment_2 segment;
};

//class PathPrimitiveCircle : public PathPrimitive {
//  public:
//    PathPrimitiveCircle() : PathPrimitive() {}

//    PathPrimitiveCircle( QPointF center, QPointF start, QPointF end, bool anyDirection, int passNumber )
//      : PathPrimitive( anyDirection, 0, passNumber ),
//        center( center ), start( start ), end( end ) {}

//    virtual Type getType() override{
//      return Type::Circle;
//    }

//  public:
//    virtual double distanceToPoint( const Point_2& ) override;

//  public:
//    QPointF center;
//    QPointF start;
//    QPointF end;
//};

#endif // PATHPRIMITIVE_H
