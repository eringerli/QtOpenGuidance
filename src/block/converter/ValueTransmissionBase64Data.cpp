// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionBase64Data.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>

#include <QBasicTimer>
#include <QBrush>
#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionBase64Data::ValueTransmissionBase64Data( uint16_t cid ) : ValueTransmissionBase( cid ) {
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

QNEBlock*
ValueTransmissionBase64DataFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValueTransmissionBase64Data( id );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( dataChanged( const QByteArray& ) ) ), false );

  b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ), false );
  b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

  b->setBrush( converterColor );

  return b;
}
