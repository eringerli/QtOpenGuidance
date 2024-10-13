// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CommunicationJrk.h"

#include <QByteArray>

void
CommunicationJrk::setSteeringAngle( double steeringAngle, const CalculationOption::Options ) {
  QByteArray data;
  data.resize( 2 );

  int16_t target = int16_t( steeringAngle * countsPerDegree ) + int16_t( steerZero );
  data[0]        = char( 0xc0 | ( target & 0x1f ) );
  data[1]        = char( ( target >> 5 ) & 0x7f );

  Q_EMIT dataReceived( data );
}

void
CommunicationJrk::setSteerZero( double steerZero, const CalculationOption::Options ) {
  this->steerZero = steerZero;
}

void
CommunicationJrk::setSteerCountPerDegree( double countsPerDegree, const CalculationOption::Options ) {
  this->countsPerDegree = countsPerDegree;
}

std::unique_ptr< BlockBase >
CommunicationJrkFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< CommunicationJrk >( idHint );

  obj->addInputPort( QStringLiteral( "Steerzero" ), obj.get(), QLatin1StringView( SLOT( setSteerZero( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Steering count/°" ), obj.get(), QLatin1StringView( SLOT( setSteerCountPerDegree( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  return obj;
}
