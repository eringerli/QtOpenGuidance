// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TrailerKinematic.h"

#include "qneblock.h"
#include "qneport.h"

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

QNEBlock*
TrailerKinematicFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TrailerKinematic();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Offset Hook to Pivot" ), QLatin1String( SLOT( setOffsetHookToPivot( VECTOR_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Offset Pivot To Tow" ), QLatin1String( SLOT( setOffsetPivotToTow( VECTOR_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Initial Mpc Pose Pivot" ), QLatin1String( SLOT( setPoseInitialMpcPivot( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
