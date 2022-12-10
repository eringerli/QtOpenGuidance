// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValueTransmissionQuaternion.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>

#include <QBasicTimer>
#include <QBrush>
#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>

ValueTransmissionQuaternion::ValueTransmissionQuaternion( int id ) : ValueTransmissionBase( id ) {}

void
ValueTransmissionQuaternion::setQuaternion( const Eigen::Quaterniond& quaternion ) {
  QCborMap map;
  map[QStringLiteral( "channelId" )] = id;
  map[QStringLiteral( "x" )]         = quaternion.x();
  map[QStringLiteral( "y" )]         = quaternion.y();
  map[QStringLiteral( "z" )]         = quaternion.z();
  map[QStringLiteral( "w" )]         = quaternion.w();

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void
ValueTransmissionQuaternion::dataReceive( const QByteArray& data ) {
  reader->addData( data );

  auto cbor = QCborValue::fromCbor( *reader );

  if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {
    auto x = cbor[QStringLiteral( "x" )].toDouble( 0 );
    auto y = cbor[QStringLiteral( "y" )].toDouble( 0 );
    auto z = cbor[QStringLiteral( "z" )].toDouble( 0 );
    auto w = cbor[QStringLiteral( "w" )].toDouble( 0 );

    Q_EMIT quaternionChanged( Eigen::Quaterniond( w, x, y, z ) );
  }
}

QNEBlock*
ValueTransmissionQuaternionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValueTransmissionQuaternion( id );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( quaternionChanged( const Eigen::Quaterniond ) ) ), false );

  b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setQuaternion( const Eigen::Quaterniond ) ) ), false );
  b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

  b->setBrush( converterColor );

  return b;
}
