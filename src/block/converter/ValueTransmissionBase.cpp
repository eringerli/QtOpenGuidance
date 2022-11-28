// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionBase.h"

#include <QBasicTimer>
#include <QBrush>
#include <QJsonObject>
#include <QTimerEvent>

ValueTransmissionBase::ValueTransmissionBase( const int id ) : BlockBase(), id( id ) {
  timeoutTimer = std::make_unique< QBasicTimer >();
  repeatTimer  = std::make_unique< QBasicTimer >();
}

QJsonObject
ValueTransmissionBase::toJSON() {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "id" )]            = id;
  valuesObject[QStringLiteral( "timeoutTimeMs" )] = timeoutTimeMs;
  valuesObject[QStringLiteral( "repeatTimeMs" )]  = repeatTimeMs;
  return valuesObject;
}

void
ValueTransmissionBase::fromJSON( QJsonObject& json ) {
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

void
ValueTransmissionBase::setTimeoutTimeMs( int value ) {
  timeoutTimeMs = value;
}

void
ValueTransmissionBase::setRepeatTimeMs( int value ) {
  repeatTimeMs = value;
}

void
ValueTransmissionBase::setTransmissionId( int id ) {
  this->id = id;
}

void
ValueTransmissionBase::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == timeoutTimer->timerId() ) {
    Q_EMIT timedOut( id );
  }

  if( event->timerId() == repeatTimer->timerId() ) {
    retransmit();
  }
}

void
ValueTransmissionBase::resetTimeout() {
  timeoutTimer->start( timeoutTimeMs, this );
}

void
ValueTransmissionBase::retransmit() {}
