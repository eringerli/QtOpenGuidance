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

#include <memory>

#include "block/BlockBase.h"

class QBasicTimer;

class ValueTransmissionBase : public BlockBase {
    Q_OBJECT

  public:
    explicit ValueTransmissionBase( const int id );

    void toJSON( QJsonObject& json ) override;
    void fromJSON( QJsonObject& json ) override;

  public Q_SLOTS:
    void setTimeoutTimeMs( int value );
    void setRepeatTimeMs( int value );

    void setTransmissionId( int id );

  Q_SIGNALS:
    void timedOut( int id );

  protected:
    void timerEvent( QTimerEvent* event ) override;
    void resetTimeout();
    virtual void retransmit();

  public:
    int id = 0;
    int timeoutTimeMs = 1000;
    int repeatTimeMs = 0;

  private:
    std::unique_ptr<QBasicTimer> timeoutTimer;
    std::unique_ptr<QBasicTimer> repeatTimer;
};
