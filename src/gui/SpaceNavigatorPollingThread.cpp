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
