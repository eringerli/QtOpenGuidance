// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef TRACTORMODEL_H
#define TRACTORMODEL_H

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QSphereMesh>

#include "BlockBase.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class TractorModel : public BlockBase {
    Q_OBJECT

  public:
    explicit TractorModel( Qt3DCore::QEntity* rootEntity );
    ~TractorModel();

  public slots:
    void setPoseHookPoint( const Point_3&, const QQuaternion, const PoseOption::Options );
    void setPoseTowPoint( const Point_3&, const QQuaternion, const PoseOption::Options );
    void setPosePivotPoint( const Point_3&, const QQuaternion, const PoseOption::Options );

    void setSteeringAngleLeft( float steerAngle );
    void setSteeringAngleRight( float steerAngle );

    void setWheelbase( float wheelbase );
    void setTrackwidth( float trackwidth );

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

    Qt3DExtras::QCuboidMesh*  m_baseMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_wheelFrontMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_wheelBackMesh = nullptr;

    Qt3DExtras::QSphereMesh*  m_towHookMesh = nullptr;
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
    TractorModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Tractor Model" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new TractorModel( rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( float ) ) ) );
      b->addInputPort( QStringLiteral( "Track Width" ), QLatin1String( SLOT( setTrackwidth( float ) ) ) );

      b->addInputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SLOT( setPoseHookPoint( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SLOT( setPosePivotPoint( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SLOT( setPoseTowPoint( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );

      b->addInputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SLOT( setSteeringAngleLeft( float ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SLOT( setSteeringAngleRight( float ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // TRACTORMODEL_H
