// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionBase.h"

#include <QBasicTimer>

#include <QJsonObject>
#include <QTimerEvent>

ValueTransmissionBase::ValueTransmissionBase( const uint16_t cid, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ), cid( cid ) {
  timeoutTimer = std::make_unique< QBasicTimer >();
  repeatTimer  = std::make_unique< QBasicTimer >();
}

void
ValueTransmissionBase::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "cid" )]           = cid;
  valuesObject[QStringLiteral( "timeoutTimeMs" )] = timeoutTimeMs;
  valuesObject[QStringLiteral( "repeatTimeMs" )]  = repeatTimeMs;
}

void
ValueTransmissionBase::fromJSON( const QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "cid" )].isDouble() ) {
    cid = valuesObject[QStringLiteral( "cid" )].toInt();
  }

  if( valuesObject[QStringLiteral( "timeoutTimeMs" )].isDouble() ) {
    timeoutTimeMs = valuesObject[QStringLiteral( "timeoutTimeMs" )].toInt();
  }

  if( valuesObject[QStringLiteral( "repeatTimeMs" )].isDouble() ) {
    repeatTimeMs = valuesObject[QStringLiteral( "repeatTimeMs" )].toInt();
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
