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

#ifndef PATHPRIMITIVE_H
#define PATHPRIMITIVE_H

#include <QObject>

#include <QLineF>
#include <QPointF>
#include <QtMath>

#include "Tile.h"

#include "../cgal.h"

#include <QDebug>

class PathPrimitive : public QObject {
    Q_OBJECT

  public:
    PathPrimitive() {}

    PathPrimitive( bool anyDirection, int passNumber )
      : anyDirection( anyDirection ), passNumber( passNumber ) {}

  public:
    virtual qreal distanceToPoint( const QPointF point ) = 0;

    static K::Point_2 getPoint2FromTiledPosition( Tile* tile, QVector3D position ) {
      return K::Point_2( tile->x + double( position.x() ),
                         tile->y + double( position.y() ) );
    }

    static constexpr QPointF getQPointFFromTiledPosition( Tile* tile, QVector3D position ) {
      return QPointF( tile->x + double( position.x() ),
                      tile->y + double( position.y() ) );
    }

  public:
    bool anyDirection = false;
    int passNumber = 0;
};

class PathPrimitiveLine : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveLine()
      : PathPrimitive() {}

    PathPrimitiveLine( QLineF line, qreal width, bool isSegment, bool anyDirection, int passNumber )
      : PathPrimitive( anyDirection, passNumber ), line( line ), width( width ), isSegment( isSegment ) {
    }

  public:
    void reverse() {
      QLineF tmpLine = line;
      line.setPoints( tmpLine.p2(), tmpLine.p1() );
    }

  public:
    virtual qreal distanceToPoint( const QPointF point ) override {
      return lineToPointDistance2D(
                     line.x1(), line.y1(),
                     line.x2(), line.y2(),
                     point.x(), point.y(),
                     isSegment
             );
    }

  public:
    QLineF line;
    qreal width = 0;
    bool isSegment = false;

  private:

    // https://stackoverflow.com/a/4448097

    // Compute the dot product AB . BC
    double dotProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cX - bX ) + ( bY - aY ) * ( cY - bY );
    }

    // Compute the cross product AB x AC
    double crossProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cY - aY ) - ( bY - aY ) * ( cX - aX );
    }

    // Compute the distance from A to B
    double distance( double aX, double aY, double bX, double bY ) {
      double d1 = aX - bX;
      double d2 = aY - bY;

      return qSqrt( d1 * d1 + d2 * d2 );
    }

    // Compute the distance from AB to C
    // if isSegment is true, AB is a segment, not a line.
    // if <0: left side of line
    double lineToPointDistance2D( double aX, double aY, double bX, double bY, double cX, double cY, bool isSegment ) {
      if( isSegment ) {
        if( dotProduct( aX, aY, bX, bY, cX, cY ) > 0 ) {
          return distance( bX, bY, cX, cY );
        }

        if( dotProduct( bX, bY, aX, aY, cX, cY ) > 0 ) {
          return distance( aX, aY, cX, cY );
        }
      }

      return crossProduct( aX, aY, bX, bY, cX, cY ) / distance( aX, aY, bX, bY );
    }

};

class PathPrimitiveCircle : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveCircle() : PathPrimitive() {}

    PathPrimitiveCircle( QPointF center, QPointF start, QPointF end, bool anyDirection, int passNumber )
      : PathPrimitive( anyDirection, passNumber ),
        center( center ), start( start ), end( end ) {}

  public:
    virtual qreal distanceToPoint( const QPointF ) override {
      return 0;
    }

  public:
    QPointF center;
    QPointF start;
    QPointF end;
};

#endif // PATHPRIMITIVE_H
