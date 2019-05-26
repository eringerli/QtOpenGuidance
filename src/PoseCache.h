
#ifndef POSECACHE_H
#define POSECACHE_H

#include <QtCore/QObject>

#include <QQuaternion>
#include <QVector3D>

#include "GuidanceBase.h"

#include "qneblock.h"
#include "qneport.h"

class PoseCache : public GuidanceBase {
    Q_OBJECT

  public:
    explicit PoseCache()
      : GuidanceBase(),
        steeringAngle(), position(), orientation() {}
    ~PoseCache() {}

  public slots:
    void setPosition( QVector3D value ) {
      position = value;
      emit poseChanged( position, orientation );
    }

    void setOrientation( QQuaternion value ) {
      orientation = value;
    }

    void setSteeringAngle( float value ) {
      steeringAngle = value;
      emit steeringAngleChanged( steeringAngle );
    }

  signals:
    void poseChanged( QVector3D position, QQuaternion orientation );
    void steeringAngleChanged( float );

  public:
    float steeringAngle;
    QVector3D position;
    QQuaternion orientation;
};

class PoseCacheFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    PoseCacheFactory()
      : GuidanceFactory() {}
    ~PoseCacheFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Pose Cache" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new PoseCache;
    }

    virtual void createBlock( QGraphicsScene* scene, GuidanceBase* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Cache", "", 0, QNEPort::NamePort );
      b->addPort( "Pose Cache", "", 0, QNEPort::TypePort );

      b->addInputPort( "Position", SLOT( setPosition( QVector3D ) ) );
      b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );
      b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );

      b->addOutputPort( "Pose", SIGNAL( poseChanged( QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Steering Angle", SIGNAL( steeringAngleChanged( float ) ) );
    }
};

#endif // POSECACHE_H

