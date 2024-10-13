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
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include "helpers/cgalHelper.h"

class BufferMesh;
class BufferMeshWithNormals;

#include <utility>

class FieldModel : public BlockBase {
  Q_OBJECT

public:
  explicit FieldModel( Qt3DCore::QEntity* rootEntity, const bool usePBR, const int idHint, const bool systemBlock, const QString type );

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

  void refreshColors();

public Q_SLOTS:
  void setVisible( const bool );

  void setPoints( const std::vector< Epick::Point_3 >& );
  void addPoint( const Eigen::Vector3d& );
  void clearPoints();
  void setField( std::shared_ptr< Polygon_with_holes_2 > field );
  void clearField();

private:
  bool visible = true;

  QColor perimeterColor = QColor( Qt::red );
  QColor pointsColor    = QColor( Qt::darkBlue );

private:
  Qt3DCore::QEntity*                    baseEntity           = nullptr;
  Qt3DCore::QTransform*                 baseTransform        = nullptr;
  Qt3DCore::QEntity*                    perimeterEntity      = nullptr;
  Qt3DCore::QEntity*                    pointsEntity         = nullptr;
  BufferMesh*                           perimeterMesh        = nullptr;
  BufferMesh*                           pointsMesh           = nullptr;
  Qt3DExtras::QDiffuseSpecularMaterial* perimeterMaterialDS  = nullptr;
  Qt3DExtras::QMetalRoughMaterial*      perimeterMaterialPBR = nullptr;
  Qt3DExtras::QDiffuseSpecularMaterial* pointsMaterialDS     = nullptr;
  Qt3DExtras::QMetalRoughMaterial*      pointsMaterialPBR    = nullptr;

  QVector< QVector3D > points;

private:
};
