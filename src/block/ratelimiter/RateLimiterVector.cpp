// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterVector.h"

#include <QBrush>
#include <iostream>

#include "qneblock.h"
#include "qneport.h"

QNEBlock*
RateLimiterVectorFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new RateLimiterVector();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Rate" ), QLatin1String( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Vector" ), QLatin1String( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Vector" ), QLatin1String( SIGNAL( vectorChanged( VECTOR_SIGNATURE ) ) ) );

  b->setBrush( converterColor );

  return b;
}

void
RateLimiterVector::setRate( const double rate, const CalculationOption::Options ) {
  if( rate > 0.01 ) {
    interval = 1. / rate;
  }
}

void
RateLimiterVector::setVector( const Eigen::Vector3d& vector, const CalculationOption::Options option ) {
  if( rateLimiter.expired( interval ) ) {
    Q_EMIT vectorChanged( vector, option );
  }
}
