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

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLevelOfDetail>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QColor>
#include <QtMath>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

#include "3d/BufferMesh.h"

class GridModel : public BlockBase {
    Q_OBJECT

  public:
    explicit GridModel( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity );

    ~GridModel();

  public Q_SLOTS:
    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& );

    void setGrid( bool enabled );

    void setGridValues( const float xStep,
                        const float yStep,
                        const float xStepCoarse,
                        const float yStepCoarse,
                        const float size,
                        const float cameraThreshold,
                        const float cameraThresholdCoarse,
                        const QColor color,
                        const QColor colorCoarse );

    void  currentIndexChanged( const int currentIndex );

  Q_SIGNALS:

  private:
    Qt3DCore::QEntity* m_distanceMeasurementEntity = nullptr;
    Qt3DCore::QTransform* m_distanceMeasurementTransform = nullptr;
    Qt3DRender::QLevelOfDetail* m_lod = nullptr;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    Qt3DCore::QEntity* m_fineGridEntity = nullptr;
    Qt3DCore::QEntity* m_coarseGridEntity = nullptr;
    BufferMesh* m_fineLinesMesh = nullptr;
    BufferMesh* m_coarseLinesMesh = nullptr;
    Qt3DExtras::QPhongMaterial* m_material = nullptr;
    Qt3DExtras::QPhongMaterial* m_materialCoarse = nullptr;

    double xStep = 1;
    double yStep = 1;
    double xStepCoarse = 10;
    double yStepCoarse = 10;
    double xStepMax = 1;
    double yStepMax = 1;
};

class GridModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    GridModelFactory( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : BlockFactory(),
        rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Grid Model" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Graphical" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DRender::QCamera* m_cameraEntity = nullptr;
};
