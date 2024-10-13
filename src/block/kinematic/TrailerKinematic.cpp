// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TrailerKinematic.h"

#include "helpers/anglesHelper.h"

void
TrailerKinematic::setOffsetHookToPivot( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
  hookToPivot.setOffset( offset );
}

void
TrailerKinematic::setOffsetPivotToTow( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
  pivotToTow.setOffset( offset );
}

void
TrailerKinematic::setMaxJackknifeAngle( const double maxAngle, CalculationOption::Options ) {
  hookToPivot.setMaxJackknifeAngle( maxAngle );
}

void
TrailerKinematic::setMaxAngle( const double maxAngle, CalculationOption::Options ) {
  hookToPivot.setMaxAngle( maxAngle );
}

void
TrailerKinematic::setPose( const Eigen::Vector3d&           position,
                           const Eigen::Quaterniond&        orientation,
                           const CalculationOption::Options options ) {
  hookToPivot.setPose( position, orientation, options );
  pivotToTow.setPose( hookToPivot.positionCalculated, hookToPivot.orientationCalculated, options );

  Q_EMIT poseHookPointChanged( position, orientation, options );
  Q_EMIT posePivotPointChanged( hookToPivot.positionCalculated, hookToPivot.orientationCalculated, options );
  Q_EMIT poseTowPointChanged( pivotToTow.positionCalculated, pivotToTow.orientationCalculated, options );
}

void
TrailerKinematic::setPoseInitialMpcPivot( const Eigen::Vector3d&           position,
                                          const Eigen::Quaterniond&        orientation,
                                          const CalculationOption::Options options ) {
  hookToPivot.setPoseInitialMpcPivot( position, orientation, options );
}

std::unique_ptr< BlockBase >
TrailerKinematicFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< TrailerKinematic >( idHint );

  obj->addInputPort(
    QStringLiteral( "Offset Hook to Pivot" ), obj.get(), QLatin1StringView( SLOT( setOffsetHookToPivot( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Offset Pivot To Tow" ), obj.get(), QLatin1StringView( SLOT( setOffsetPivotToTow( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "MaxJackknifeAngle" ), obj.get(), QLatin1StringView( SLOT( setMaxJackknifeAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "MaxAngle" ), obj.get(), QLatin1StringView( SLOT( setMaxAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Initial Mpc Pose Pivot" ), obj.get(), QLatin1StringView( SLOT( setPoseInitialMpcPivot( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Pose Hook Point" ), obj.get(), QLatin1StringView( SIGNAL( poseHookPointChanged( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Pose Pivot Point" ), obj.get(), QLatin1StringView( SIGNAL( posePivotPointChanged( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Pose Tow Point" ), obj.get(), QLatin1StringView( SIGNAL( poseTowPointChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
