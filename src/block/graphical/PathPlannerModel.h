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

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QAttribute>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

#include "kinematic/Plan.h"

class BufferMesh;

#include <utility>

class PathPlannerModel : public BlockBase {
    Q_OBJECT

  public:
    explicit PathPlannerModel( Qt3DCore::QEntity* rootEntity );

    void toJSON( QJsonObject& json ) override;
    void fromJSON( QJsonObject& json ) override;

    void refreshColors();

  public Q_SLOTS:
    void setVisible( const bool visible );

    void setPlan( const Plan& plan );

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options );

  public:
    Eigen::Vector3d position = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();

    bool visible = true;
    double zOffset = 0.1;
    double viewBox = 50;
    bool individualSegmentColor = false;
    bool individualRayColor = false;
    bool bisectorsVisible = false;

    QColor linesColor = QColor( Qt::green );
    QColor segmentsColor = QColor( Qt::green );
    QColor raysColor = QColor( Qt::green );
    QColor bisectorsColor = QColor( Qt::blue );

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;

    Qt3DCore::QEntity* baseEntity = nullptr;

    Qt3DCore::QEntity* linesEntity = nullptr;
    Qt3DCore::QEntity* raysEntity = nullptr;
    Qt3DCore::QEntity* segmentsEntity = nullptr;
    Qt3DCore::QEntity* bisectorsEntity = nullptr;
    BufferMesh* linesMesh = nullptr;
    BufferMesh* raysMesh = nullptr;
    BufferMesh* segmentsMesh = nullptr;
    BufferMesh* bisectorsMesh = nullptr;
    Qt3DExtras::QPhongMaterial* linesMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* raysMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* bisectorsMaterial = nullptr;

  private:
    Plan plan;
};

class PathPlannerModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    PathPlannerModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Path Planner Model" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Graphical" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};
