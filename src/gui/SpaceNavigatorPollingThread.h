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

#ifndef SPACENAVIGATORPOLLINGTHREAD_H
#define SPACENAVIGATORPOLLINGTHREAD_H

#include <QObject>

#include <QtGlobal>
#include <QtDebug>

// do not include the config of SPNAV, so X11 doesn't get pulled in...
#define SPNAV_CONFIG_H_

#include <spnav.h>

// mainly from here:
// https://github.com/KDE/calligra/tree/master/plugins/spacenavigator

#include <QThread>

class SpaceNavigatorPollingThread : public QThread {
    Q_OBJECT
  public:
    explicit SpaceNavigatorPollingThread( QObject* parent )
      : QThread( parent ), m_stopped( false ) {}

    void stop() {
      m_stopped = true;
    }

  signals:
    void steerAngleChanged( float );
    void velocityChanged( float );

  protected:

    // reimplemented from QThread
    virtual void run() {
      m_stopped = false;

      if( spnav_open() == -1 ) {
        return;
      }

      while( ! m_stopped ) {
        spnav_event event;

        if( spnav_poll_event( &event ) ) {
          if( event.type == SPNAV_EVENT_MOTION ) {

            // The coordinate system of the space navigator is like the following:
            // x-axis : from left to right
            // y-axis : from down to up
            // z-axis : from front to back

            // the min/max is around +-400
            // use forward tilt for speed and left/right for the steering
            float steerAngle = float( event.motion.ry ) / 400 * 45;
            float velocity = float( event.motion.rx ) / 400 * -15;

            emit steerAngleChanged( steerAngle );
            emit velocityChanged( velocity );
          }

          spnav_remove_events( event.type );
        }

        msleep( 10 );
      }
    }

  private:
    bool m_stopped;
};

#endif // SPACENAVIGATORPOLLINGTHREAD_H
