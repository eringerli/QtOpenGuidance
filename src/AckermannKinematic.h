
#ifndef ACKERMANNKINEMATIC_H
#define ACKERMANNKINEMATIC_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

#include <QtGlobal>
#include <QtDebug>

class AckermannKinematic : public QObject {
    Q_OBJECT

  public:
    explicit AckermannKinematic()
      : m_enabled( true ), m_steerAngle( 0 ), m_velocity( 0 ), m_wheelbase( 2.4 ), m_position(), m_orientation() {
    }
    ~AckermannKinematic() {}

    bool enabled() {
      return m_enabled;
    }

    float steerAngle() {
      return m_steerAngle;
    }
    float velocity() {
      return m_velocity;
    }
    float wheelbase() {
      return m_wheelbase;
    }
    QQuaternion orientation() {
      return m_orientation;
    }
    QVector3D position() {
      return m_position;
    }


  public slots:
    void setEnabled( bool enabled ) {
      m_enabled = enabled;
      emit enableChanged( m_enabled );
    }

    void setSteerAngle( float steerAngle ) {
      m_steerAngle = steerAngle;
//       qDebug() << "setSteerAngle(float): " << steerAngle;
      emit steerAngleChanged( m_steerAngle );
    }

    void setWheelbase( float wheelbase ) {
      m_wheelbase = wheelbase;
      emit wheelbaseChanged( m_wheelbase );
    }

    void setTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
      emit towPointPositionChanged(position);
    }

    void setOwnHitch( QVector3D position ) {
      m_offsetOwnHitch = position;
      emit hitchPositionChanged(position);
    }

    void setPose( QVector3D position, QQuaternion orientation, float steeringAngle );

  signals:
    void enableChanged( bool );

    void steerAngleChanged( float );
    void wheelbaseChanged( float );

    void hitchPositionChanged( QVector3D );
    void towPointPositionChanged( QVector3D );
    void poseChanged( QVector3D, QQuaternion, float );

    void positionChangedRelative( QVector3D );
    void orientationChangedRelative( QQuaternion );
  private:
    bool m_enabled;

    QTime m_time;

    float m_steerAngle;
    float m_velocity;
    float m_wheelbase;

    QVector3D m_offsetTowPoint;
    QVector3D m_offsetOwnHitch;

    QVector3D m_position;
    QQuaternion m_orientation;
};

#endif // ACKERMANNKINEMATIC_H

