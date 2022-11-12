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
