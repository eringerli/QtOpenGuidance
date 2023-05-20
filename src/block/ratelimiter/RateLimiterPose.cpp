// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RateLimiterPose.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

QNEBlock*
RateLimiterPoseFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new RateLimiterPose();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Rate" ), QLatin1String( SLOT( setRate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  b->setBrush( converterColor );

  return b;
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
