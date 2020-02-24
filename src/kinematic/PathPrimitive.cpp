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

#include "PathPrimitive.h"

#include <QObject>

#include <QLineF>
#include <QPointF>
#include <QtMath>

#include "../cgalKernel.h"

#include <QDebug>

void PathPrimitive::print() {
  std::cout << "PathPrimitive" << std::endl;
}

void PathPrimitiveLine::reverse() {
  line = line.opposite();
}
void PathPrimitiveSegment::reverse() {
  segment = segment.opposite();
}

double PathPrimitiveSegment::distanceToPoint( const Point_2& point ) {
  Point_2 ortogonalProjection = segment.supporting_line().projection( point );
  double distance = sqrt( CGAL::squared_distance( ortogonalProjection, point ) );

  std::cout << "s:" << segment << "p:" << point << "op:" << ortogonalProjection << std::endl;

  qDebug() << "Distance:" << distance;

  if( segment.collinear_has_on( ortogonalProjection ) ) {
    qDebug() << "segment.has_on";
    return distance;
  } else {
    double distToSource = CGAL::squared_distance( point, segment.source() );
    double distToTarget = CGAL::squared_distance( point, segment.target() );
    qDebug() << distToSource << distToTarget;

    if( distToSource < distToTarget ) {
      return sqrt( distToSource );
    } else {
      return sqrt( distToTarget );
    }
  }
}

void PathPrimitiveSegment::print() {
  std::cout << "PathPrimitiveSegment: " << segment << std::endl;
}

double PathPrimitiveLine::distanceToPoint( const Point_2& point ) {
  Point_2 ortogonalProjection = line.projection( point );
  double distance = sqrt( CGAL::squared_distance( ortogonalProjection, point ) );

  std::cout << "l:" << line << "p:" << point << "op:" << ortogonalProjection << std::endl;

  qDebug() << "Distance:" << distance;

  return distance;
}

void PathPrimitiveLine::print() {
  std::cout << "PathPrimitiveLine: " << line << std::endl;
}

//qreal PathPrimitiveCircle::distanceToPoint( const Point_2& ) {
//  return 0;
//}

const PathPrimitiveLine* PathPrimitive::castToLine() {
  if( getType() == Type::Line ) {
    return static_cast<PathPrimitiveLine*>( this );
  } else {
    return nullptr;
  }
}

const PathPrimitiveSegment* PathPrimitive::castToSegment() {
  if( getType() == Type::Segment ) {
    return static_cast<PathPrimitiveSegment*>( this );
  } else {
    return nullptr;
  }
}

//const PathPrimitiveCircle* PathPrimitive::castToCircle() {
//  if( getType() == Type::Circle ) {
//    return static_cast<PathPrimitiveCircle*>( this );
//  } else {
//    return nullptr;
//  }
//}
