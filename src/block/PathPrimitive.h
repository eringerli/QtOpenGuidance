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

#include <QtWidgets>
#include <QComboBox>

#include <QJsonObject>

#include <Qt3DCore/QEntity>

#include "../kinematic/Tile.h"

class PathPrimitive : public QObject {
    Q_OBJECT

  public:
    PathPrimitive() {}
};

class PathPrimitiveLine : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveLine( double x1, double y1, double x2, double y2, bool segment )
      : PathPrimitive(),
        x1( x1 ), y1( y1 ), x2( x2 ), y2( y2 ), segment( segment ) {
      qDebug() << "PathPrimitiveLine()";
    }

    ~PathPrimitiveLine() {
      qDebug() << "~PathPrimitiveLine()";
    }

  public:
    double x1, y1;
    double x2, y2;
    bool segment;
};

class PathPrimitiveCircle : public PathPrimitive {
    Q_OBJECT

  public:
    PathPrimitiveCircle( double x1, double y1, double x2, double y2, double xCenter, double yCenter )
      : PathPrimitive(),
        x1( x1 ), y1( y1 ), x2( x2 ), y2( y2 ), xCenter( xCenter ), yCenter( yCenter ) {}

  public:
    double x1, y1;
    double x2, y2;
    double xCenter, yCenter;
};

#endif // PATHPRIMITIVE_H
