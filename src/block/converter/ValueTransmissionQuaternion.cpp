// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "ValueTransmissionQuaternion.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>

#include <QCborMap>
#include <QCborStreamReader>
#include <QCborValue>
#include <QBrush>
#include <QBasicTimer>

ValueTransmissionQuaternion::ValueTransmissionQuaternion( int id ) : ValueTransmissionBase( id ) {}

void ValueTransmissionQuaternion::setQuaternion( const Eigen::Quaterniond& quaternion ) {
  QCborMap map;
  map[QStringLiteral( "channelId" )] = id;
  map[QStringLiteral( "x" )] = quaternion.x();
  map[QStringLiteral( "y" )] = quaternion.y();
  map[QStringLiteral( "z" )] = quaternion.z();
  map[QStringLiteral( "w" )] = quaternion.w();

  Q_EMIT dataToSend( QCborValue( std::move( map ) ).toCbor() );
}

void ValueTransmissionQuaternion::dataReceive( const QByteArray& data ) {
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

QNEBlock* ValueTransmissionQuaternionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValueTransmissionQuaternion( id );
  auto* b = createBaseBlock( scene, obj, id, false );

  b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
  b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( quaternionChanged( const Eigen::Quaterniond ) ) ), false );

  b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setQuaternion( const Eigen::Quaterniond ) ) ), false );
  b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

  b->setBrush( converterColor );

  return b;
}
