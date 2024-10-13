// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterPose.h"

std::unique_ptr< BlockBase >
RateLimiterPoseFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< RateLimiterPose >( idHint );

  obj->addInputPort( QStringLiteral( "Rate" ), obj.get(), QLatin1StringView( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}

void
RateLimiterPose::setRate( const double rate, const CalculationOption::Options ) {
  if( rate > 0.01 ) {
    interval = 1. / rate;
  }
}

void
RateLimiterPose::setPose( const Eigen::Vector3d&           position,
                          const Eigen::Quaterniond&        orientation,
                          const CalculationOption::Options option ) {
  if( rateLimiter.expired( interval ) ) {
    Q_EMIT poseChanged( position, orientation, option );
  }
}
