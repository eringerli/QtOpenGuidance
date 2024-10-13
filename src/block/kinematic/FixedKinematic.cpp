// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FixedKinematic.h"

void
FixedKinematic::setOffsetHookToPivot( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
  hookToPivot.setOffset( offset );
}

void
FixedKinematic::setOffsetPivotToTow( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
  pivotToTow.setOffset( offset );
}

void
FixedKinematic::setPose( const Eigen::Vector3d&           position,
                         const Eigen::Quaterniond&        orientation,
                         const CalculationOption::Options options ) {
  hookToPivot.setPose( position, orientation, options );
  pivotToTow.setPose( hookToPivot.positionCalculated, hookToPivot.orientationCalculated, options );

  Q_EMIT poseHookPointChanged( position, orientation, options );
  Q_EMIT posePivotPointChanged( hookToPivot.positionCalculated, hookToPivot.orientationCalculated, options );
  Q_EMIT poseTowPointChanged( pivotToTow.positionCalculated, pivotToTow.orientationCalculated, options );
}

std::unique_ptr< BlockBase >
FixedKinematicFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< FixedKinematic >( idHint );

  obj->addInputPort(
    QStringLiteral( "Offset Hook to Pivot" ), obj.get(), QLatin1StringView( SLOT( setOffsetHookToPivot( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Offset Pivot To Tow" ), obj.get(), QLatin1StringView( SLOT( setOffsetPivotToTow( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Pose Hook Point" ), obj.get(), QLatin1StringView( SIGNAL( poseHookPointChanged( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Pose Pivot Point" ), obj.get(), QLatin1StringView( SIGNAL( posePivotPointChanged( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Pose Tow Point" ), obj.get(), QLatin1StringView( SIGNAL( poseTowPointChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
