// Copyright( C ) 2019 Christian Riggenbach
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
    float elapsedTime = float ( m_time.restart() ) / 1000;
    QQuaternion lastOrientation = m_orientation;

    emit steeringAngleChanged( m_steerAngle );

    // heading
    {
      QQuaternion difference = QQuaternion::fromEulerAngles( QVector3D(
                                 0,
                                 0,
                                 qRadiansToDegrees( elapsedTime * ( qTan( qDegreesToRadians( m_steerAngle ) / m_wheelbase * m_velocity ) ) )
                               ) );
      m_orientation *=  difference;
      emit orientationChanged( m_orientation );
    }

    // local position
    {
      float headingRad = qDegreesToRadians( lastOrientation.toEulerAngles().z() );
      QVector3D difference = QVector3D(
                               elapsedTime * qCos( headingRad ) * m_velocity,
                               elapsedTime * qSin( headingRad ) * m_velocity,
                               0
                             );
      m_position += difference;

      // emit signal with antenna offset
      emit positionChanged( m_position - m_orientation * ( -m_antennaPosition ) );
    }

    // in global coordinates: WGS84
    {
      constexpr double R = 6378388; // Earth Radius in m
      double distanceR = ( elapsedTime * m_velocity ) / R;

      double cosDistanceR = qCos( distanceR );
      double sinDistanceR = qSin( distanceR );
      double cosLatitude = qCos( latitude );
      double sinLatitude = qSin( latitude );

      double headingInRad = qDegreesToRadians( m_orientation.toEulerAngles().z());

      latitude = qAsin( ( sinLatitude * cosDistanceR ) +
                           ( cosLatitude * sinDistanceR * qCos( headingInRad ) ) );

      longitude += qAtan2( qSin( headingInRad ) * sinDistanceR * cosLatitude,
                           cosDistanceR - ( sinLatitude * qSin( latitude ) ) );

      emit globalPositionChanged( qRadiansToDegrees( latitude ), qRadiansToDegrees( longitude ), 450 );
//      qDebug() << "lat, lon" << qRadiansToDegrees( latitude ) << qRadiansToDegrees( longitude );
    }

  }
}
