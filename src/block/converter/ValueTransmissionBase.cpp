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

#include "ValueTransmissionBase.h"


#include <QJsonObject>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QBrush>

ValueTransmissionBase::ValueTransmissionBase( const int id )
  : BlockBase(), id( id ) {
  timeoutTimer = std::make_unique<QBasicTimer>();
  repeatTimer = std::make_unique<QBasicTimer>();
}

void ValueTransmissionBase::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "id" )] = id;
  valuesObject[QStringLiteral( "timeoutTimeMs" )] = timeoutTimeMs;
  valuesObject[QStringLiteral( "repeatTimeMs" )] = repeatTimeMs;
  json[QStringLiteral( "values" )] = valuesObject;
}

void ValueTransmissionBase::fromJSON( QJsonObject& json ) {
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

void ValueTransmissionBase::setTimeoutTimeMs( int value ) {
  timeoutTimeMs = value;
}

void ValueTransmissionBase::setRepeatTimeMs( int value ) {
  repeatTimeMs = value;
}

void ValueTransmissionBase::setTransmissionId( int id ) {
  this->id = id;
}

void ValueTransmissionBase::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == timeoutTimer->timerId() ) {
    Q_EMIT timedOut( id );
  }

  if( event->timerId() == repeatTimer->timerId() ) {
    retransmit();
  }
}

void ValueTransmissionBase::resetTimeout() {
  timeoutTimer->start( timeoutTimeMs, this );
}

void ValueTransmissionBase::retransmit() {}
