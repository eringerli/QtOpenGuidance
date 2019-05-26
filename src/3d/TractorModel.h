
#ifndef TRACTORENTITY_H
#define TRACTORENTITY_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include "../GuidanceBase.h"

class TractorModel : public GuidanceBase {
    Q_OBJECT

  public:
    explicit TractorModel( Qt3DCore::QEntity* rootEntity );
    ~TractorModel();

    QVector3D position();
    QQuaternion rotation();

  public slots:
    void setPoseHookPoint( QVector3D, QQuaternion );
    void setPoseTowPoint( QVector3D, QQuaternion );
    void setPosePivotPoint( QVector3D, QQuaternion );

    void setSteeringAngle( float steerAngle );
    void setWheelbase( float wheelbase );

  signals:
    void positionChanged( QVector3D position );
    void rotationChanged( QQuaternion rotation );

  private:
    Qt3DCore::QEntity* m_rootEntity;

    Qt3DCore::QEntity* m_baseEntity;
    Qt3DCore::QEntity* m_wheelFrontLeftEntity;
    Qt3DCore::QEntity* m_wheelFrontRightEntity;
    Qt3DCore::QEntity* m_wheelBackLeftEntity;
    Qt3DCore::QEntity* m_wheelBackRightEntity;

    Qt3DCore::QEntity* m_towHookEntity;
    Qt3DCore::QEntity* m_pivotPointEntity;
    Qt3DCore::QEntity* m_towPointEntity;

    Qt3DExtras::QCuboidMesh*  m_baseMesh;
    Qt3DExtras::QCylinderMesh* m_wheelFrontMesh;
    Qt3DExtras::QCylinderMesh* m_wheelBackMesh;

    Qt3DExtras::QSphereMesh*  m_towHookMesh;
    Qt3DExtras::QSphereMesh* m_pivotPointMesh;
    Qt3DExtras::QSphereMesh* m_towPointMesh;

    Qt3DCore::QTransform* m_rootEntityTransform;
    Qt3DCore::QTransform* m_baseTransform;
    Qt3DCore::QTransform* m_wheelFrontLeftTransform;
    Qt3DCore::QTransform* m_wheelFrontRightTransform;
    Qt3DCore::QTransform* m_wheelBackLeftTransform;
    Qt3DCore::QTransform* m_wheelBackRightTransform;

    Qt3DCore::QTransform* m_towHookTransform;
    Qt3DCore::QTransform* m_pivotPointTransform;
    Qt3DCore::QTransform* m_towPointTransform;

    float m_wheelbase;
    float m_steeringAngle{};
};

class TractorModelFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    TractorModelFactory( Qt3DCore::QEntity* rootEntity )
      : GuidanceFactory(),
        rootEntity( rootEntity ) {}
    ~TractorModelFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Tractor Model" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new TractorModel( rootEntity );
    }

    virtual void createBlock( QGraphicsScene* scene, GuidanceBase* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Tractor", "", 0, QNEPort::NamePort );
      b->addPort( "Tractor Model", "", 0, QNEPort::TypePort );

      b->addInputPort( "Pose Hook Point", SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Pose Pivot Point", SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Pose Tow Point", SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
      b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
    }

  private:
    Qt3DCore::QEntity* rootEntity;
};

#endif // TRACTORENTITY_H

