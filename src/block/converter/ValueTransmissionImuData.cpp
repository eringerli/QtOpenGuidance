// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionImuData.h"

#include "qcborarray.h"

#include <QByteArray>

#include <QBasicTimer>

#include <QCborMap>
#include <QCborValue>
#include <iostream>

ValueTransmissionImuData::ValueTransmissionImuData( uint16_t cid, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : ValueTransmissionBase( cid, idHint, systemBlock, type ) {
  reader = std::make_unique< QCborStreamReader >();
}

void
ValueTransmissionImuData::setImuData( const double dT, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d& ) {}

void
ValueTransmissionImuData::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "cid" )] == cid ) ) {
    auto imuData = cbor[QStringLiteral( "imu" )].toArray();

    for( const auto& item : std::as_const( imuData ) ) {
      if( item.isMap() ) {
        uint32_t ts   = item[QStringLiteral( "ts" )].toInteger();
        double   dT   = ( double )( ts - lastTimestamp ) / 1e6;
        lastTimestamp = ts;

        if( item[QStringLiteral( "ag" )].isArray() ) {
          const auto&     data = item[QStringLiteral( "ag" )].toArray();
          Eigen::Vector3d acc( data[0].toDouble(), data[1].toDouble(), data[2].toDouble() );
          Eigen::Vector3d gyr( data[3].toDouble(), data[4].toDouble(), data[5].toDouble() );
          Eigen::Vector3d mag( 0, 0, 0 );

          Q_EMIT imuDataChanged( dT, acc, gyr, mag );
        }
        if( item[QStringLiteral( "agm" )].isArray() ) {
          const auto&     data = item[QStringLiteral( "agm" )].toArray();
          Eigen::Vector3d acc( data[0].toDouble(), data[1].toDouble(), data[2].toDouble() );
          Eigen::Vector3d gyr( data[3].toDouble(), data[4].toDouble(), data[5].toDouble() );
          Eigen::Vector3d mag( data[6].toDouble(), data[7].toDouble(), data[8].toDouble() );
          Q_EMIT imuDataChanged( dT, acc, gyr, mag );
        }
      }
    }
  }
}

std::unique_ptr< BlockBase >
ValueTransmissionImuDataFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ValueTransmissionImuData >( idHint, 1000 );

  obj->addInputPort( QStringLiteral( "CBOR In" ), obj.get(), QLatin1StringView( SLOT( dataReceive( const QByteArray& ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Out" ),
    obj.get(),
    QLatin1StringView( SIGNAL( imuDataChanged( const double, const Eigen::Vector3d&, const Eigen::Vector3d&, const Eigen::Vector3d& ) ) ) );

  obj->addOutputPort( QStringLiteral( "CBOR Out" ), obj.get(), QLatin1StringView( SIGNAL( dataToSend( const QByteArray& ) ) ) );

  return obj;
}
