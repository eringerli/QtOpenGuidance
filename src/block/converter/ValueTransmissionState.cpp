// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionState.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>

#include <QBasicTimer>
#include <QBrush>
#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionState::ValueTransmissionState( uint16_t cid ) : ValueTransmissionBase( cid ) {
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

QNEBlock*
ValueTransmissionStateFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValueTransmissionState( id );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ), false );

  b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setState( ACTION_SIGNATURE ) ) ), false );
  b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

  b->setBrush( converterColor );

  return b;
}
