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

#include <QObject>
#include <Qt3DRender/QGeometryRenderer>

#include "3d/qt3dForwards.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

class CgalThread;
class CultivatedAreaMeshGeometry;

class CultivatedAreaMesh : public Qt3DRender::QGeometryRenderer {
  Q_OBJECT

public:
  explicit CultivatedAreaMesh( Qt3DCore::QNode* parent = nullptr );
  ~CultivatedAreaMesh();

  void addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector );
  void addPointLeft( const Point_3 point, const QVector3D normalVector );
  void addPointRight( const Point_3 point, const QVector3D normalVector );

  void clear();

  void addTrackMesh( CultivatedAreaMesh* trackMesh );

  void optimise();
};
