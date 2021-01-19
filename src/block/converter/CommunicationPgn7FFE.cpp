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

#include "CommunicationPgn7FFE.h"

#include "qneblock.h"
#include "qneport.h"

#include <QByteArray>
#include <QBrush>

void CommunicationPgn7ffe::setSteeringAngle( double steeringAngle ) {
  QByteArray data;
  data.resize( 8 );
  data[0] = char( 0x7f );
  data[1] = char( 0xfe );

  // relais
  data[2] = 0;

  // velocity in km/4h
  data[3] = char( velocity * 3.6f * 4.0f );

  // XTE in mm
  auto xte = int16_t( distance * 1000 );
  data[4] = char( xte >> 8 );
  data[5] = char( xte & 0xff );

  // steerangle in °/100
  auto steerangle = int16_t( steeringAngle * 100 );
  data[6] = char( steerangle >> 8 );
  data[7] = char( steerangle & 0xff );

  Q_EMIT dataReceived( data );
}

void CommunicationPgn7ffe::setXte( double distance ) {
  this->distance = distance;
}

void CommunicationPgn7ffe::setVelocity( double velocity ) {
  this->velocity = velocity;
}

QNEBlock* CommunicationPgn7ffeFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new CommunicationPgn7ffe();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( double ) ) ) );
  b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( double ) ) ) );
  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( parserColor );

  return b;
}
