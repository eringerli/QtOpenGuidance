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
#include <QDebug>

class PathPrimitive : public QObject {
    Q_OBJECT

  public:
    PathPrimitive() {}

    PathPrimitive( bool anyDirection )
      : anyDirection( anyDirection ) {}

  public:
    bool anyDirection = false;
};

class PathPrimitiveLine : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveLine()
      : PathPrimitive() {}

    PathPrimitiveLine( QLineF line, bool isSegment, bool anyDirection )
      : PathPrimitive( anyDirection ), line( line ), isSegment( isSegment ) {
      qDebug() << "PathPrimitiveLine()";
    }

    ~PathPrimitiveLine() {
      qDebug() << "~PathPrimitiveLine()";
    }

  public:
    QLineF line;
    bool isSegment = false;
};

class PathPrimitiveCircle : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveCircle() : PathPrimitive() {}

    PathPrimitiveCircle( QPointF center, QPointF start, QPointF end, bool anyDirection )
      : PathPrimitive( anyDirection ),
        center( center ), start( start ), end( end ) {}

  public:
    QPointF center;
    QPointF start;
    QPointF end;
};

#endif // PATHPRIMITIVE_H
