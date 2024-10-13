// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterImu.h"

std::unique_ptr< BlockBase >
RateLimiterImuFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< RateLimiterImu >( idHint );

  obj->addInputPort( QStringLiteral( "Rate" ), obj.get(), QLatin1StringView( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Imu" ), obj.get(), QLatin1StringView( SLOT( setImu( IMU_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Imu" ), obj.get(), QLatin1StringView( SIGNAL( imuChanged( IMU_SIGNATURE ) ) ) );

  return obj;
}

void
RateLimiterImu::setRate( const double rate, const CalculationOption::Options ) {
  if( rate > 0.01 ) {
    interval = 1. / rate;
  }
}

void
RateLimiterImu::setImu( const double dT, const Eigen::Vector3d& acc, const Eigen::Vector3d& gyr, const Eigen::Vector3d& mag ) {
  if( rateLimiter.expired( interval ) ) {
    Q_EMIT imuChanged( dT, acc, gyr, mag );
  }
}
