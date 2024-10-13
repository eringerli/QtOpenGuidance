// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionBase64Data.h"

#include <QByteArray>

#include <QBasicTimer>

#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionBase64Data::ValueTransmissionBase64Data( uint16_t cid, const int idHint, const bool systemBlock, const QString type )
    : ValueTransmissionBase( cid, idHint, systemBlock, type ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionBase64Data::setData( const QByteArray& data ) {
  QCborMap map;
  map[QStringLiteral( "cid" )] = cid;
  map[QStringLiteral( "d" )]   = QString::fromLatin1( data.toBase64( QByteArray::OmitTrailingEquals ) );

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionBase64Data::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "cid" )] == cid ) ) {
    Q_EMIT dataChanged( QByteArray::fromBase64( cbor[QStringLiteral( "d" )].toString().toLatin1() ) );
  }
}

std::unique_ptr< BlockBase >
ValueTransmissionBase64DataFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ValueTransmissionBase64Data >( idHint, 1000 );

  obj->addInputPort( QStringLiteral( "CBOR In" ), obj.get(), QLatin1StringView( SLOT( dataReceive( const QByteArray& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Out" ), obj.get(), QLatin1StringView( SIGNAL( dataChanged( const QByteArray& ) ) ) );

  obj->addInputPort( QStringLiteral( "In" ), obj.get(), QLatin1StringView( SLOT( setData( const QByteArray& ) ) ) );
  obj->addOutputPort( QStringLiteral( "CBOR Out" ), obj.get(), QLatin1StringView( SIGNAL( dataToSend( const QByteArray& ) ) ) );

  return obj;
}
