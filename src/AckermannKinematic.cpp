
#include "AckermannKinematic.h"

#include <QTime>
#include <QEvent>
#include <QtMath>

#include <QtDebug>

// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

//void AckermannKinematic::timerEvent( QTimerEvent* event ) {
//  if( event->timerId() == m_timer.timerId() ) {
//    float elapsedTime = ( float )m_time.restart() / 1000;
////     qDebug() << "elapsedTime: " << elapsedTime << " m_velocity: " << m_velocity << " m_wheelbase: " << m_wheelbase;
//    QQuaternion lastOrientation = m_orientation;

//    {
//      QQuaternion difference = QQuaternion::fromEulerAngles( QVector3D(
//                                 0,
//                                 0,
//                                 qRadiansToDegrees( elapsedTime * ( qTan( qDegreesToRadians( m_steerAngle ) / m_wheelbase * m_velocity ) ) )
//                               ) );
//      m_orientation *=  difference;
////       qDebug() << "Orientation: " << m_orientation;
//      emit orientationChanged( m_orientation );
//      emit orientationChangedRelative( difference );
//    }

//    {
//      float headingRad = qDegreesToRadians( lastOrientation.toEulerAngles().z() );
//      QVector3D difference = QVector3D(
//                               elapsedTime * qCos( headingRad ) * m_velocity,
//                               elapsedTime * qSin( headingRad ) * m_velocity,
//                               0
//                             );
//      m_position += difference;
////       qDebug() << "Position: " << m_position << "eulerAngles: " << eulerAngles << " headingRad:" << headingRad << " difference: " << difference;

//      // emit signal with antenna offset
//      emit positionChanged( m_orientation*m_antennaPosition );
//      emit positionChangedRelative( difference );
//    }

//  }
//}


void AckermannKinematic::setPose( QVector3D position, QQuaternion orientation, float steeringAngle ) {
  static QVector3D lastPosition = QVector3D();

  // calculate elapsed time and velocities in all 3 axis
  float elapsedTime = ( float )m_time.restart() / 1000;
  QVector3D velocity = ( position - lastPosition ) / elapsedTime;

  // calculate the radius of the virtual center of the curve if ideal ackermann-kinetics are used
  float radiusToVirtualCenterToPivotPoint = m_wheelbase * qTan( qDegreesToRadians( steeringAngle ) );

  // calculate a virtual steering wheel on the position of the point m_offsetTowPoint
  float virtualSteeringAngleTowPoint = qAtan( m_offsetTowPoint.x() / ( radiusToVirtualCenterToPivotPoint - m_offsetTowPoint.y() ) );

  // -90°
  virtualSteeringAngleTowPoint -= M_PI;

  // map the velocity to the pivot point
  velocity = QQuaternion::fromAxisAndAngle( QVector3D( 0.0f, 0.0f, 1.0f ), qRadiansToDegrees( virtualSteeringAngleTowPoint ) ) * velocity;
  velocity *= virtualSteeringAngleTowPoint / ( m_offsetOwnHitch.x() * qTan( qDegreesToRadians( steeringAngle ) ) + m_offsetOwnHitch.y() );

  // calculate new heading (world coordinates)
  m_orientation *= QQuaternion::fromAxisAndAngle(
                     QVector3D( 0.0f, 0.0f, 1.0f ),
                     qRadiansToDegrees( elapsedTime * ( qTan( virtualSteeringAngleTowPoint / m_offsetOwnHitch.x() * velocity.x() ) ) )
                   );

  m_position = position-m_orientation*(m_offsetTowPoint-m_offsetOwnHitch);

  m_orientation = orientation;
  emit poseChanged( m_position/*+QVector3D(1,1,0)*/, m_orientation, qRadiansToDegrees(virtualSteeringAngleTowPoint ));

  lastPosition = position;
}
