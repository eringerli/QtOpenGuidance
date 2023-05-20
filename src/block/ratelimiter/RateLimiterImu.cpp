// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterImu.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

QNEBlock*
RateLimiterImuFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new RateLimiterImu();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Rate" ), QLatin1String( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Imu" ), QLatin1String( SLOT( setImu( IMU_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Imu" ), QLatin1String( SIGNAL( imuChanged( IMU_SIGNATURE ) ) ) );

  b->setBrush( converterColor );

  return b;
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
