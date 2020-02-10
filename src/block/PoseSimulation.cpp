// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "PoseSimulation.h"

#include <QTime>
#include <QEvent>
#include <QtMath>

#include <QDebug>

// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

void PoseSimulation::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == m_timer.timerId() ) {
    constexpr double msPerS = 1000;
    double elapsedTime = double ( m_time.restart() ) / msPerS;
    QQuaternion lastOrientation = m_orientation;

    float steerAngle = 0;

    if( m_autosteerEnabled ) {
      steerAngle = m_steerAngleFromAutosteer;
    } else {
      steerAngle = m_steerAngle;
    }

    emit steeringAngleChanged( steerAngle );
    emit velocityChanged( m_velocity );

    // heading
    {
      QQuaternion difference = QQuaternion::fromEulerAngles( QVector3D(
                                 0,
                                 0,
                                 float( qRadiansToDegrees( elapsedTime * ( qTan( qDegreesToRadians( steerAngle ) / m_wheelbase * m_velocity ) ) ) )
                               ) );
      m_orientation *=  difference;
      emit orientationChanged( m_orientation );
    }

    // local position
    {
      double headingRad = qDegreesToRadians( double( lastOrientation.toEulerAngles().z() ) );
      double elapsedTimeVelocity = elapsedTime * double( m_velocity );
      x +=  elapsedTimeVelocity * qCos( headingRad );
      y += elapsedTimeVelocity * qSin( headingRad );
    }
    QVector3D antenna =  m_orientation * m_antennaPosition;

    double xWithAntennaOffset = x + double( antenna.x() );
    double yWithAntennaOffset = y + double( antenna.y() );
    double zWithAntennaOffset = height + double( antenna.z() );

    // emit signal with antenna offset
    emit positionChanged( QVector3D( float( xWithAntennaOffset ), float( yWithAntennaOffset ), float( zWithAntennaOffset ) ) );


    // in global coordinates: WGS84
    {
      double latitude, longitude, height;
      geographicConvertionWrapper->Reverse( xWithAntennaOffset, yWithAntennaOffset, zWithAntennaOffset, latitude, longitude, height );

      QElapsedTimer timer;
      timer.start();
      emit globalPositionChanged( latitude, longitude, height );
//      qDebug() << "Cycle Time PoseSimulation:    " << timer.nsecsElapsed() << "ns";
    }

  }
}
