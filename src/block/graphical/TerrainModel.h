// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "block/BlockBase.h"

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
  explicit TerrainModel( Qt3DCore::QEntity* rootEntity, bool usePBR, const int idHint, const bool systemBlock, const QString type );

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

  void refreshColors();

public Q_SLOTS:
  void setVisible( const bool );
  void setSurface( std::shared_ptr< SurfaceMesh_3 > );

public:
  bool visible = true;

  QColor linesColor   = QColor( Qt::yellow );
  QColor terrainColor = QColor( Qt::gray );

private:
  bool usePBR = false;

  Qt3DCore::QEntity*     baseEntity    = nullptr;
  Qt3DCore::QTransform*  baseTransform = nullptr;
  Qt3DCore::QEntity*     linesEntity   = nullptr;
  Qt3DCore::QEntity*     terrainEntity = nullptr;
  BufferMesh*            linesMesh     = nullptr;
  BufferMeshWithNormals* terrainMesh   = nullptr;

  Qt3DExtras::QPhongMaterial*      linesMaterial      = nullptr;
  Qt3DExtras::QPhongMaterial*      terrainMaterial    = nullptr;
  Qt3DExtras::QMetalRoughMaterial* linesMaterialPbr   = nullptr;
  Qt3DExtras::QMetalRoughMaterial* terrainMaterialPbr = nullptr;

private:
};
