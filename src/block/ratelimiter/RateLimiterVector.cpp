// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterVector.h"

#include <iostream>

std::unique_ptr< BlockBase >
RateLimiterVectorFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< RateLimiterVector >( idHint );

  obj->addInputPort( QStringLiteral( "Rate" ), obj.get(), QLatin1StringView( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Vector" ), obj.get(), QLatin1StringView( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Vector" ), obj.get(), QLatin1StringView( SIGNAL( vectorChanged( VECTOR_SIGNATURE ) ) ) );

  return obj;
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
