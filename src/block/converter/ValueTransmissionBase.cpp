// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionBase.h"

#include <QBasicTimer>
#include <QBrush>
#include <QJsonObject>
#include <QTimerEvent>

ValueTransmissionBase::ValueTransmissionBase( const uint16_t cid ) : BlockBase(), cid( cid ) {
  timeoutTimer = std::make_unique< QBasicTimer >();
  repeatTimer  = std::make_unique< QBasicTimer >();
}

QJsonObject
ValueTransmissionBase::toJSON() {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "cid" )]           = cid;
  valuesObject[QStringLiteral( "timeoutTimeMs" )] = timeoutTimeMs;
  valuesObject[QStringLiteral( "repeatTimeMs" )]  = repeatTimeMs;
  return valuesObject;
}

void
ValueTransmissionBase::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "cid" )].isDouble() ) {
    cid = json[QStringLiteral( "cid" )].toInt();
  }

  if( json[QStringLiteral( "timeoutTimeMs" )].isDouble() ) {
    timeoutTimeMs = json[QStringLiteral( "timeoutTimeMs" )].toInt();
  }

  if( json[QStringLiteral( "repeatTimeMs" )].isDouble() ) {
    repeatTimeMs = json[QStringLiteral( "repeatTimeMs" )].toInt();
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
ValueTransmissionBase::setTransmissionId( uint16_t id ) {
  this->cid = id;
}

void
ValueTransmissionBase::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == timeoutTimer->timerId() ) {
    Q_EMIT timedOut( cid );
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
