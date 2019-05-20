
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
    Q_PROPERTY( QVector3D position READ position WRITE setPosition NOTIFY positionChanged )
    Q_PROPERTY( QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged )

  public:
    explicit TractorModel( Qt3DCore::QEntity* rootEntity );
    ~TractorModel();

    QVector3D position();
    QQuaternion rotation();

  public slots:
    void setAntennaPosition( QVector3D position );
    void setPosition( QVector3D position );
    void setRotation( QQuaternion rotation );
    void setPose( QVector3D position, QQuaternion rotation, float steerAngle );
    void setSteerAngle( float steerAngle );
    void setWheelbase( float wheelbase );
    void setHitchPosition( QVector3D position );

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
    Qt3DCore::QEntity* m_antennaEntity;
    Qt3DCore::QEntity* m_pivotPointEntity;
    Qt3DCore::QEntity* m_hitchEntity;

    Qt3DExtras::QCuboidMesh*  m_baseMesh;
    Qt3DExtras::QCylinderMesh* m_wheelFrontLeftMesh;
    Qt3DExtras::QCylinderMesh* m_wheelFrontRightMesh;
    Qt3DExtras::QCylinderMesh* m_wheelBackLeftMesh;
    Qt3DExtras::QCylinderMesh* m_wheelBackRightMesh;
    Qt3DExtras::QSphereMesh*  m_antennaMesh;
    Qt3DExtras::QSphereMesh* m_pivotPointMesh;
    Qt3DExtras::QSphereMesh* m_hitchMesh;

    Qt3DCore::QTransform* m_rootEntityTransform;
    Qt3DCore::QTransform* m_baseTransform;
    Qt3DCore::QTransform* m_wheelFrontLeftTransform;
    Qt3DCore::QTransform* m_wheelFrontRightTransform;
    Qt3DCore::QTransform* m_wheelBackLeftTransform;
    Qt3DCore::QTransform* m_wheelBackRightTransform;
    Qt3DCore::QTransform* m_antennaTransform;
    Qt3DCore::QTransform* m_pivotPointTransform;
    Qt3DCore::QTransform* m_hitchTransform;

    QVector3D m_hitchPosition;
    float m_wheelbase;
    float m_steerangle;
    QVector3D m_antennaPosition;
};

#endif // TRACTORENTITY_H

