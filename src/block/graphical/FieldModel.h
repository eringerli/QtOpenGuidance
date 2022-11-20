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

class BufferMesh;
class BufferMeshWithNormals;

#include <utility>

class FieldModel : public BlockBase {
  Q_OBJECT

public:
  explicit FieldModel( Qt3DCore::QEntity* rootEntity );

  QJsonObject toJSON() override;
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
