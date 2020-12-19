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

#pragma once

#include <QObject>

#include <QtGlobal>

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
      : QThread( parent ) {}

    void stop() {
      m_stopped = true;
    }

  Q_SIGNALS:
    void steerAngleChanged( const double );
    void velocityChanged( const double );

  protected:

    // reimplemented from QThread
    virtual void run();

  private:
    bool m_stopped = false;
};
