// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CommunicationJrk.h"

#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
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

QNEBlock*
CommunicationJrkFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new CommunicationJrk();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Steerzero" ), QLatin1String( SLOT( setSteerZero( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering count/°" ), QLatin1String( SLOT( setSteerCountPerDegree( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( parserColor );

  return b;
}
