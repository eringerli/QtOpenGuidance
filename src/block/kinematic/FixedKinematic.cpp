// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FixedKinematic.h"

#include "qneblock.h"
#include "qneport.h"

void
FixedKinematic::setOffsetHookToPivot( const Eigen::Vector3d& offset ) {
  hookToPivot.setOffset( offset );
}

void
FixedKinematic::setOffsetPivotToTow( const Eigen::Vector3d& offset ) {
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

QNEBlock*
FixedKinematicFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FixedKinematic;
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Offset Hook to Pivot" ), QLatin1String( SLOT( setOffsetHookToPivot( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Offset Pivot To Tow" ), QLatin1String( SLOT( setOffsetPivotToTow( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
