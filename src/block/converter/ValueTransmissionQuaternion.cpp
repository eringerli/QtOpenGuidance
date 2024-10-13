// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionQuaternion.h"

#include <QByteArray>

#include <QBasicTimer>

#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionQuaternion::ValueTransmissionQuaternion( uint16_t cid, const int idHint, const bool systemBlock, const QString type )
    : ValueTransmissionBase( cid, idHint, systemBlock, type ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionQuaternion::setQuaternion( const Eigen::Quaterniond& quaternion, const CalculationOption::Options ) {
  QCborMap map;
  map[QStringLiteral( "cid" )] = cid;
  map[QStringLiteral( "x" )]   = quaternion.x();
  map[QStringLiteral( "y" )]   = quaternion.y();
  map[QStringLiteral( "z" )]   = quaternion.z();
  map[QStringLiteral( "w" )]   = quaternion.w();

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionQuaternion::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "cid" )] == cid ) ) {
    auto x = cbor[QStringLiteral( "x" )].toDouble( 0 );
    auto y = cbor[QStringLiteral( "y" )].toDouble( 0 );
    auto z = cbor[QStringLiteral( "z" )].toDouble( 0 );
    auto w = cbor[QStringLiteral( "w" )].toDouble( 0 );

    Q_EMIT quaternionChanged( Eigen::Quaterniond( w, x, y, z ), CalculationOption::Option::None );
  }
}

std::unique_ptr< BlockBase >
ValueTransmissionQuaternionFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ValueTransmissionQuaternion >( idHint, 0 );

  obj->addInputPort( QStringLiteral( "CBOR In" ), obj.get(), QLatin1StringView( SLOT( dataReceive( const QByteArray& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Out" ), obj.get(), QLatin1StringView( SIGNAL( quaternionChanged( ORIENTATION_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "In" ), obj.get(), QLatin1StringView( SLOT( setQuaternion( ORIENTATION_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "CBOR Out" ), obj.get(), QLatin1StringView( SIGNAL( dataToSend( const QByteArray& ) ) ) );

  return obj;
}
