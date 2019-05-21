
#ifndef TRAILERKINEMATIC_H
#define TRAILERKINEMATIC_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

#include <QtGlobal>
#include <QtMath>
#include <QtDebug>

class TrailerKinematic : public QObject {
    Q_OBJECT

  public:
    explicit TrailerKinematic()
      : m_enabled( true ), m_positionPivotPoint( QVector3D( -1, 0, 0 ) ) {
    }
    ~TrailerKinematic() {}

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
      QQuaternion orientationTrailer = QQuaternion::fromAxisAndAngle(
                                         QVector3D( 0.0f, 0.0f, 1.0f ),
                                         qRadiansToDegrees( qAtan2( position.y() - m_positionPivotPoint.y(), position.x() - m_positionPivotPoint.x() ) )
                                       );

      // the angle between tractor and trailer >120° -> reset orientation to the one from the tractor
      if( qAbs( ( orientation.inverted()*orientationTrailer ).toEulerAngles().z() ) < 120 ) {
        orientation = orientationTrailer;
      }

      m_positionPivotPoint = position + orientation * -m_offsetHookPoint;
      QVector3D positionTowPoint = m_positionPivotPoint + orientation * m_offsetTowPoint;

      emit poseHookPointChanged( position, orientation );
      emit posePivotPointChanged( m_positionPivotPoint, orientation );
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

    QVector3D m_positionPivotPoint;
};

#endif // TRAILERKINEMATIC_H
