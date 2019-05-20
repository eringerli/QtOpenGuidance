
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

class TractorModel : public QObject {
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
    float m_steeringAngle;
};

#endif // TRACTORENTITY_H

