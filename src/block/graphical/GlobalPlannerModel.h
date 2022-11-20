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

#include "kinematic/Plan.h"

class BufferMesh;

#include <utility>

class GlobalPlannerModel : public BlockBase {
  Q_OBJECT

public:
  explicit GlobalPlannerModel( Qt3DCore::QEntity* rootEntity );

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

  void refreshColors();

public Q_SLOTS:
  void setVisible( const bool visible );
  void setSizeOfPoint( const float sizeOfPoint );

  void setPose( POSE_SIGNATURE_SLOT );
  void showPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polylinePtr );

public:
  Eigen::Vector3d    position    = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientation = Eigen::Quaterniond( 0, 0, 0, 0 );

  bool   visible     = true;
  float  sizeOfPoint = 1.0f;
  QColor aColor      = QColor( QColorConstants::Svg::orange );
  QColor bColor      = QColor( QColorConstants::Svg::orange );
  QColor pointColor  = QColor( QColorConstants::Svg::purple );

private:
  Qt3DCore::QEntity*    baseEntity    = nullptr;
  Qt3DCore::QTransform* baseTransform = nullptr;

  Qt3DCore::QEntity*          aPointEntity    = nullptr;
  Qt3DExtras::QSphereMesh*    aPointMesh      = nullptr;
  Qt3DCore::QTransform*       aPointTransform = nullptr;
  Qt3DCore::QEntity*          aTextEntity     = nullptr;
  Qt3DCore::QTransform*       aTextTransform  = nullptr;
  Qt3DExtras::QPhongMaterial* aMaterial       = nullptr;

  Qt3DCore::QEntity*          bPointEntity    = nullptr;
  Qt3DExtras::QSphereMesh*    bPointMesh      = nullptr;
  Qt3DCore::QTransform*       bPointTransform = nullptr;
  Qt3DCore::QEntity*          bTextEntity     = nullptr;
  Qt3DCore::QTransform*       bTextTransform  = nullptr;
  Qt3DExtras::QPhongMaterial* bMaterial       = nullptr;

  Qt3DCore::QEntity*          pointsEntity   = nullptr;
  Qt3DExtras::QSphereMesh*    pointsMesh     = nullptr;
  Qt3DExtras::QPhongMaterial* pointsMaterial = nullptr;

private:
};
