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

#include "CommunicationJrk.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>
#include <QBrush>

void CommunicationJrk::setSteeringAngle( double steeringAngle ) {
  QByteArray data;
  data.resize( 2 );

  int16_t target = int16_t( steeringAngle * countsPerDegree ) + int16_t( steerZero );
  data[0] = char( 0xc0 | ( target & 0x1f ) );
  data[1] = char( ( target >> 5 ) & 0x7f );

  Q_EMIT dataReceived( data );
}

void CommunicationJrk::setSteerZero( double steerZero ) {
  this->steerZero = steerZero;
}

void CommunicationJrk::setSteerCountPerDegree( double countsPerDegree ) {
  this->countsPerDegree = countsPerDegree;
}

QNEBlock* CommunicationJrkFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new CommunicationJrk();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Steerzero" ), QLatin1String( SLOT( setSteerZero( double ) ) ) );
  b->addInputPort( QStringLiteral( "Steering count/°" ), QLatin1String( SLOT( setSteerCountPerDegree( double ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( parserColor );

  return b;
}
