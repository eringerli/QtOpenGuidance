
#ifndef POSESIMULATION_H
#define POSESIMULATION_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QtGui/qquaternion.h>
#include <QtGui/qvector3d.h>

#include <QtGlobal>
#include <QtDebug>

class PoseSimulation : public QObject {
    Q_OBJECT

    Q_PROPERTY( bool enabled READ isSimulation WRITE setSimulation NOTIFY simulationChanged )
    Q_PROPERTY( int interval READ interval WRITE setInterval NOTIFY intervalChanged )

    Q_PROPERTY( float steerAngle READ steerAngle WRITE setSteerAngle NOTIFY steerAngleChanged )
    Q_PROPERTY( float velocity READ velocity WRITE setVelocity NOTIFY velocityChanged )
    Q_PROPERTY( float wheelbase READ wheelbase WRITE setWheelbase NOTIFY wheelbaseChanged )

    Q_PROPERTY( QVector3D position READ position WRITE setPosition NOTIFY positionChanged )
    Q_PROPERTY( QQuaternion orientation READ orientation WRITE setOrientation NOTIFY orientationChanged )
    Q_PROPERTY( QVector3D antennaPosition READ antennaPosition WRITE setAntennaPosition NOTIFY antennaPositionChanged )

  public:
    explicit PoseSimulation()
      : m_enabled( false ), m_interval( 50 ), m_timerId( 0 ), m_steerAngle( 0 ), m_velocity( 0 ), m_wheelbase( 2.4 ), m_position(), m_orientation() {
      setSimulation( false );
    }
    ~PoseSimulation() {}


    bool isSimulation() {
      return m_enabled;
    }
    int interval() {
      return m_interval;
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
    QVector3D antennaPosition() {
      return m_antennaPosition;
    }


  public slots:
    void setInterval( int interval ) {
      m_interval = interval;

      setSimulation( m_enabled );

      emit intervalChanged( m_interval );
    }

    void setFrequency( int frequency ) {
      setInterval( 1000 / frequency );
    }

    void setSimulation( bool enabled ) {
      m_enabled = enabled;

      if( enabled ) {
        m_timer.start( m_interval, Qt::PreciseTimer, this );
        m_time.start();
      } else {
        m_timer.stop();
      }

      emit simulationChanged( m_enabled );
    }

    void setSteerAngle( float steerAngle ) {
      m_steerAngle = steerAngle;
//       qDebug() << "setSteerAngle(float): " << steerAngle;
      emit steerAngleChanged( m_steerAngle );
    }

    void setVelocity( float velocity ) {
      m_velocity = velocity;
      emit velocityChanged( velocity );
    }

    void setWheelbase( float wheelbase ) {
      m_wheelbase = wheelbase;
      emit wheelbaseChanged( m_wheelbase );
    }

    void setAntennaPosition( QVector3D position ) {
      m_antennaPosition = position;
      emit antennaPositionChanged( m_position );
    }

    void setPosition( QVector3D position ) {
      m_position = position;
      emit positionChanged( m_position );
    }

    void setOrientation( QQuaternion orientation ) {
      m_orientation = orientation;
      emit orientationChanged( m_orientation );
    }


  protected:
    void timerEvent( QTimerEvent* event ) override;

  signals:
    void simulationChanged( bool enabled );
    void intervalChanged( int interval );

    void steerAngleChanged( float steerAngle );
    void velocityChanged( float velocity );
    void wheelbaseChanged( float wheelbase );

    void antennaPositionChanged( QVector3D position );

    void steeringAngleChanged( float );
    void positionChanged( QVector3D position );
    void positionChangedRelative( QVector3D position );
    void orientationChanged( QQuaternion orientation );
    void orientationChangedRelative( QQuaternion orientation );

  private:
    bool m_enabled;
    int m_interval;

    QBasicTimer m_timer;
    int m_timerId;
    QTime m_time;

    float m_steerAngle;
    float m_velocity;
    float m_wheelbase;

    QVector3D m_antennaPosition;
    QVector3D m_position;
    QQuaternion m_orientation;
};

#endif // POSESIMULATION_H

