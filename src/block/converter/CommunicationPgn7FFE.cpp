// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CommunicationPgn7FFE.h"

#include <QByteArray>

void
CommunicationPgn7ffe::setSteeringAngle( double steeringAngle, const CalculationOption::Options ) {
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
  data[4]  = char( xte >> 8 );
  data[5]  = char( xte & 0xff );

  // steerangle in °/100
  auto steerangle = int16_t( steeringAngle * 100 );
  data[6]         = char( steerangle >> 8 );
  data[7]         = char( steerangle & 0xff );

  Q_EMIT dataReceived( data );
}

void
CommunicationPgn7ffe::setXte( double distance, const CalculationOption::Options ) {
  this->distance = distance;
}

void
CommunicationPgn7ffe::setVelocity( double velocity, const CalculationOption::Options ) {
  this->velocity = velocity;
}

std::unique_ptr< BlockBase >
CommunicationPgn7ffeFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< CommunicationPgn7ffe >( idHint );

  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Velocity" ), obj.get(), QLatin1StringView( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "XTE" ), obj.get(), QLatin1StringView( SLOT( setXte( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  return obj;
}
