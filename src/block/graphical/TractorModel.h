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

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QSphereMesh>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class TractorModel : public BlockBase {
    Q_OBJECT

  public:
    explicit TractorModel( Qt3DCore::QEntity* rootEntity, const bool usePBR );
    ~TractorModel();

  public Q_SLOTS:
    void setPoseHookPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void setPoseTowPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void setPosePivotPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

    void setSteeringAngleLeft( const double steerAngle );
    void setSteeringAngleRight( const double steerAngle );

    void setWheelbase( const double wheelbase );
    void setTrackwidth( const double trackwidth );

  private:
    void setProportions();

    Qt3DCore::QEntity* m_rootEntity = nullptr;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QEntity* m_wheelFrontLeftEntity = nullptr;
    Qt3DCore::QEntity* m_wheelFrontRightEntity = nullptr;
    Qt3DCore::QEntity* m_wheelBackLeftEntity = nullptr;
    Qt3DCore::QEntity* m_wheelBackRightEntity = nullptr;

    Qt3DCore::QEntity* m_towHookEntity = nullptr;
    Qt3DCore::QEntity* m_pivotPointEntity = nullptr;
    Qt3DCore::QEntity* m_towPointEntity = nullptr;

    Qt3DExtras::QCuboidMesh* m_baseMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_wheelFrontMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_wheelBackMesh = nullptr;

    Qt3DExtras::QSphereMesh* m_towHookMesh = nullptr;
    Qt3DExtras::QSphereMesh* m_pivotPointMesh = nullptr;
    Qt3DExtras::QSphereMesh* m_towPointMesh = nullptr;

    Qt3DCore::QTransform* m_rootEntityTransform = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;
    Qt3DCore::QTransform* m_wheelFrontLeftTransform = nullptr;
    Qt3DCore::QTransform* m_wheelFrontRightTransform = nullptr;
    Qt3DCore::QTransform* m_wheelBackLeftTransform = nullptr;
    Qt3DCore::QTransform* m_wheelBackRightTransform = nullptr;

    Qt3DCore::QTransform* m_towHookTransform = nullptr;
    Qt3DCore::QTransform* m_pivotPointTransform = nullptr;
    Qt3DCore::QTransform* m_towPointTransform = nullptr;

    float m_wheelbase = 2.4f;
    float m_trackwidth = 2;
};

class TractorModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    TractorModelFactory( Qt3DCore::QEntity* rootEntity, const bool usePBR )
      : BlockFactory(),
        rootEntity( rootEntity ),
        usePBR( usePBR ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Tractor Model" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Graphical" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new TractorModel( rootEntity, usePBR );
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Track Width" ), QLatin1String( SLOT( setTrackwidth( const double ) ) ) );

      b->addInputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SLOT( setPoseHookPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SLOT( setPosePivotPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SLOT( setPoseTowPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      b->addInputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SLOT( setSteeringAngleLeft( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SLOT( setSteeringAngleRight( const double ) ) ) );

      b->setBrush( modelColor );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    bool usePBR = true;
};
