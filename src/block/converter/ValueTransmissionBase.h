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

#include "block/BlockBase.h"

class ValueTransmissionBase : public BlockBase {
    Q_OBJECT

  public:
    explicit ValueTransmissionBase( const int id )
      : BlockBase(), id( id ) {}

    virtual ~ValueTransmissionBase() {
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "id" )] = id;
      valuesObject[QStringLiteral( "timeoutTimeMs" )] = timeoutTimeMs;
      valuesObject[QStringLiteral( "repeatTimeMs" )] = repeatTimeMs;
      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "id" )].isDouble() ) {
          id = valuesObject[QStringLiteral( "id" )].toInt();
        }

        if( valuesObject[QStringLiteral( "timeoutTimeMs" )].isDouble() ) {
          timeoutTimeMs = valuesObject[QStringLiteral( "timeoutTimeMs" )].toInt();
        }

        if( valuesObject[QStringLiteral( "repeatTimeMs" )].isDouble() ) {
          repeatTimeMs = valuesObject[QStringLiteral( "repeatTimeMs" )].toInt();
        }
      }
    }

  public Q_SLOTS:
    void setTimeoutTimeMs( int value ) {
      timeoutTimeMs = value;
    }
    void setRepeatTimeMs( int value ) {
      repeatTimeMs = value;
    }

    void setTransmissionId( int id ) {
      this->id = id;
    }

  Q_SIGNALS:
    void timedOut( int id );

  protected:
    void timerEvent( QTimerEvent* event ) override;
    void resetTimeout() {
      timeoutTimer.start( timeoutTimeMs, this );
    }
    virtual void retransmit() {

    }

  public:
    int id = 0;
    int timeoutTimeMs = 1000;
    int repeatTimeMs = 0;

  private:
    QBasicTimer timeoutTimer;
    QBasicTimer repeatTimer;
};
