// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionNumber.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>

#include <QBasicTimer>
#include <QBrush>
#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionNumber::ValueTransmissionNumber( int id ) : ValueTransmissionBase( id ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionNumber::setNumber( const double number, CalculationOption::Options ) {
  QCborMap map;
  map[QStringLiteral( "channelId" )] = id;
  map[QStringLiteral( "number" )]    = number;

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionNumber::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {
    Q_EMIT numberChanged( cbor[QStringLiteral( "number" )].toDouble( 0 ), CalculationOption::Option::None );
  }
}

QNEBlock*
ValueTransmissionNumberFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValueTransmissionNumber( id );
  auto* b   = createBaseBlock( scene, obj, id, false );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ), false );

  b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setNumber( NUMBER_SIGNATURE ) ) ), false );
  b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

  b->setBrush( converterColor );

  return b;
}
