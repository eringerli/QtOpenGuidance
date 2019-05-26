
#ifndef TRACTORKINEMATIC_H
#define TRACTORKINEMATIC_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

#include <QtGlobal>
#include <QtDebug>

#include "../GuidanceBase.h"

class FixedKinematic : public GuidanceBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : GuidanceBase(),
        m_offsetHookPoint( QVector3D( 0, 0, 0 ) ), m_offsetTowPoint( QVector3D( -1, 0, 0 ) ) {}
    ~FixedKinematic() {}

  public slots:
    void setOffsetTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( QVector3D position ) {
      m_offsetHookPoint = position;
    }

    void setPose( QVector3D position, QQuaternion orientation ) {
      QVector3D positionPivotPoint = position + orientation * -m_offsetHookPoint;
      QVector3D positionTowPoint = positionPivotPoint + orientation * m_offsetTowPoint;

      emit poseHookPointChanged( position, orientation );
      emit posePivotPointChanged( positionPivotPoint, orientation );
      emit poseTowPointChanged( positionTowPoint, orientation );
    }

  signals:
    void poseHookPointChanged( QVector3D, QQuaternion );
    void posePivotPointChanged( QVector3D, QQuaternion );
    void poseTowPointChanged( QVector3D, QQuaternion );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint;
    QVector3D m_offsetTowPoint;
};

class FixedKinematicFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    FixedKinematicFactory()
      : GuidanceFactory() {}
    ~FixedKinematicFactory() {}

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( QStringLiteral( "Fixed Kinematics" ), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new FixedKinematic;
    }

    virtual void createBlock( QGraphicsScene* scene, GuidanceBase* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( "Fixed", "", 0, QNEPort::NamePort );
      b->addPort( "Fixed Kinematics", "", 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( QVector3D, QQuaternion ) ) );

      b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ) );
    }
};

#endif // TRACTORKINEMATIC_H
