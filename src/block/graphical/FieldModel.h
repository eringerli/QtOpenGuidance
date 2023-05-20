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

class BufferMesh;
class BufferMeshWithNormals;

#include <utility>

class FieldModel : public BlockBase {
  Q_OBJECT

public:
  explicit FieldModel( Qt3DCore::QEntity* rootEntity );

  QJsonObject toJSON() const override;
  void        fromJSON( QJsonObject& ) override;

  void refreshColors();

public Q_SLOTS:
  void setVisible( const bool );

  void setPoints( const std::vector< Epick::Point_3 >& );
  void addPoint( const Eigen::Vector3d& );
  void clearPoints();
  void setField( std::shared_ptr< Polygon_with_holes_2 > field );
  void clearField();

public:
  bool visible = true;

  QColor perimeterColor = QColor( Qt::green );
  QColor pointsColor    = QColor( Qt::blue );

private:
  Qt3DCore::QEntity*          baseEntity        = nullptr;
  Qt3DCore::QTransform*       baseTransform     = nullptr;
  Qt3DCore::QEntity*          perimeterEntity   = nullptr;
  Qt3DCore::QEntity*          pointsEntity      = nullptr;
  BufferMesh*                 perimeterMesh     = nullptr;
  BufferMesh*                 pointsMesh        = nullptr;
  Qt3DExtras::QPhongMaterial* perimeterMaterial = nullptr;
  Qt3DExtras::QPhongMaterial* pointsMaterial    = nullptr;

  QVector< QVector3D > points;

private:
};
