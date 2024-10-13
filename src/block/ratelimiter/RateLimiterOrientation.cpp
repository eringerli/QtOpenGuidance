// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterOrientation.h"

std::unique_ptr< BlockBase >
RateLimiterOrientationFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< RateLimiterOrientation >( idHint );

  obj->addInputPort( QStringLiteral( "Rate" ), obj.get(), QLatin1StringView( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  return obj;
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
