
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

class FixedKinematic : public QObject {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : m_enabled( true ) {
    }
    ~FixedKinematic() {}

    bool enabled() {
      return m_enabled;
    }

  public slots:
    void setEnabled( bool enabled ) {
      m_enabled = enabled;
      emit enableChanged( m_enabled );
    }

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
    void enableChanged( bool );

    void poseHookPointChanged( QVector3D, QQuaternion );
    void posePivotPointChanged( QVector3D, QQuaternion );
    void poseTowPointChanged( QVector3D, QQuaternion );
  private:
    bool m_enabled;

    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint;
    QVector3D m_offsetTowPoint;
};

#endif // TRACTORKINEMATIC_H
