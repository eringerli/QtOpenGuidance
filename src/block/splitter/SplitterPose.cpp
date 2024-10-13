// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterPose.h"

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/Quaternion.h"

void
SplitterPose::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  Q_EMIT positionChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
  Q_EMIT orientationChanged( Eigen::Quaterniond( 0, 0, 0, 0 ), CalculationOption::Option::None );
}

void
SplitterPose::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options ) {
  Q_EMIT positionChanged( position, CalculationOption::Option::None );
  Q_EMIT orientationChanged( orientation, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
SplitterPoseFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< SplitterPose >( idHint );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Position" ), obj.get(), QLatin1StringView( SIGNAL( positionChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
