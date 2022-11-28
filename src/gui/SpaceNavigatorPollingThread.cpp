// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SpaceNavigatorPollingThread.h"

void
SpaceNavigatorPollingThread::run() {
  m_stopped = false;

  if( spnav_open() == -1 ) {
    return;
  }

  while( !m_stopped ) {
    spnav_event event;

    if( spnav_poll_event( &event ) != 0 ) {
      if( event.type == SPNAV_EVENT_MOTION ) {
        // The coordinate system of the space navigator is like the following:
        // x-axis : from left to right
        // y-axis : from down to up
        // z-axis : from front to back

        // the min/max is around +-400
        // use forward tilt for speed and left/right for the steering
        double steerAngle = float( event.motion.ry ) / 400 * 45;
        double velocity   = float( event.motion.rx ) / 400 * -15;

        //            qDebug() << event.motion.rx << event.motion.ry << event.motion.rz <<
        //            event.motion.x << event.motion.y << event.motion.z;

        steerAngle = std::max( std::min( steerAngle, 35.0 ), -35.0 );
        velocity   = std::max( std::min( velocity, 10.0 ), -10.0 );

        Q_EMIT steerAngleChanged( steerAngle, CalculationOption::Option::None );
        Q_EMIT velocityChanged( velocity, CalculationOption::Option::None );
      }

      spnav_remove_events( event.type );
    }

    msleep( 10 );
  }
}
