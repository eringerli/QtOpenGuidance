// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionState.h"

#include <QByteArray>

#include <QBasicTimer>

#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionState::ValueTransmissionState( uint16_t cid, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : ValueTransmissionBase( cid, idHint, systemBlock, type ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionState::setState( const bool state ) {
  QCborMap map;
  map[QStringLiteral( "cid" )] = cid;
  map[QStringLiteral( "s" )]   = state;

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionState::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "cid" )] == cid ) ) {
    Q_EMIT stateChanged( cbor[QStringLiteral( "s" )].toBool( false ) );
  }
}

std::unique_ptr< BlockBase >
ValueTransmissionStateFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ValueTransmissionState >( idHint, 0 );

  obj->addInputPort( QStringLiteral( "CBOR In" ), obj.get(), QLatin1StringView( SLOT( dataReceive( const QByteArray& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Out" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "In" ), obj.get(), QLatin1StringView( SLOT( setState( ACTION_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "CBOR Out" ), obj.get(), QLatin1StringView( SIGNAL( dataToSend( const QByteArray& ) ) ) );

  return obj;
}
