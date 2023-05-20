// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterOrientation.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

QNEBlock*
RateLimiterOrientationFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new RateLimiterOrientation();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Rate" ), QLatin1String( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  b->setBrush( converterColor );

  return b;
}

void
RateLimiterOrientation::setRate( const double rate, const CalculationOption::Options ) {
  if( rate > 0.01 ) {
    interval = 1. / rate;
  }
}

void
RateLimiterOrientation::setOrientation( const Eigen::Quaterniond& orientation, const CalculationOption::Options option ) {
  if( rateLimiter.expired( interval ) ) {
    Q_EMIT orientationChanged( orientation, option );
  }
}
