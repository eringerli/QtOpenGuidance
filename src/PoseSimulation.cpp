
#include "PoseSimulation.h"

#include <QTime>
#include <QEvent>
#include <QtMath>

#include <QtDebug>

// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

void PoseSimulation::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == m_timer.timerId() ) {
    float elapsedTime = ( float )m_time.restart() / 1000;
//     qDebug() << "elapsedTime: " << elapsedTime << " m_velocity: " << m_velocity << " m_wheelbase: " << m_wheelbase;
    QQuaternion lastOrientation = m_orientation;

    emit steeringAngleChanged(m_steerAngle);

    {
      QQuaternion difference = QQuaternion::fromEulerAngles( QVector3D(
                                 0,
                                 0,
                                 qRadiansToDegrees( elapsedTime * ( qTan( qDegreesToRadians( m_steerAngle ) / m_wheelbase * m_velocity ) ) )
                               ) );
      m_orientation *=  difference;
//       qDebug() << "Orientation: " << m_orientation;
      emit orientationChanged( m_orientation );
      emit orientationChangedRelative( difference );
    }

    {
      float headingRad = qDegreesToRadians( lastOrientation.toEulerAngles().z() );
      QVector3D difference = QVector3D(
                               elapsedTime * qCos( headingRad ) * m_velocity,
                               elapsedTime * qSin( headingRad ) * m_velocity,
                               0
                             );
      m_position += difference;
//       qDebug() << "Position: " << m_position << " difference: " << difference;

      // emit signal with antenna offset
      emit positionChanged( m_position - m_orientation*m_antennaPosition );
      emit positionChangedRelative( difference );
    }

  }
}
