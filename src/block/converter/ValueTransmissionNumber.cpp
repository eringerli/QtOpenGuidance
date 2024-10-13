// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionNumber.h"

#include <QByteArray>

#include <QBasicTimer>

#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionNumber::ValueTransmissionNumber( uint16_t cid, const int idHint, const bool systemBlock, const QString type )
    : ValueTransmissionBase( cid, idHint, systemBlock, type ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionNumber::setNumber( const double number, CalculationOption::Options ) {
  QCborMap map;
  map[QStringLiteral( "cid" )] = cid;
  map[QStringLiteral( "n" )]   = number;

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionNumber::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "cid" )] == cid ) ) {
    Q_EMIT numberChanged( cbor[QStringLiteral( "n" )].toDouble( 0 ), CalculationOption::Option::None );
  }
}

std::unique_ptr< BlockBase >
ValueTransmissionNumberFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ValueTransmissionNumber >( idHint, 1000 );

  obj->addInputPort( QStringLiteral( "CBOR In" ), obj.get(), QLatin1StringView( SLOT( dataReceive( const QByteArray& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Out" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "In" ), obj.get(), QLatin1StringView( SLOT( setNumber( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "CBOR Out" ), obj.get(), QLatin1StringView( SIGNAL( dataToSend( const QByteArray& ) ) ) );

  return obj;
}
