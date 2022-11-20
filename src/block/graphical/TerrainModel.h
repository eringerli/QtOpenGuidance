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

#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"

#include "helpers/cgalHelper.h"

#include <CGAL/Surface_mesh.h>
using SurfaceMesh_3 = CGAL::Surface_mesh< Point_3 >;

class BufferMesh;
class BufferMeshWithNormals;

#include <utility>

class TerrainModel : public BlockBase {
  Q_OBJECT

public:
  explicit TerrainModel( Qt3DCore::QEntity* rootEntity );

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

  void refreshColors();

public Q_SLOTS:
  void setVisible( const bool );
  void setSurface( std::shared_ptr< SurfaceMesh_3 > );

public:
  bool visible = true;

  QColor linesColor   = QColor( Qt::darkGreen );
  QColor terrainColor = QColor( Qt::green );

private:
  Qt3DCore::QEntity*          baseEntity          = nullptr;
  Qt3DCore::QTransform*       baseTransform       = nullptr;
  Qt3DCore::QEntity*          linesEntity         = nullptr;
  Qt3DCore::QEntity*          terrainEntity       = nullptr;
  BufferMesh*                 linesMesh           = nullptr;
  BufferMeshWithNormals*      terrainMesh         = nullptr;
  Qt3DExtras::QPhongMaterial* linesMaterial       = nullptr;
  Qt3DExtras::QPhongMaterial* terrainMaterial     = nullptr;

private:
};

// class TerrainModelFactory : public BlockFactory {
//   Q_OBJECT

// public:
//   TerrainModelFactory( QThread* thread, Qt3DCore::QEntity* rootEntity ) : BlockFactory( thread ), rootEntity( rootEntity ) {}

//  QString getNameOfFactory() override { return QStringLiteral( "Path Planner Model" ); }

//  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

//  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

// private:
//   Qt3DCore::QEntity* rootEntity = nullptr;
// };
