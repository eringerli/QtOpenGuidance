// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FixedKinematicPrimitive.h"

#include "qneblock.h"
#include "qneport.h"

void
FixedKinematicPrimitive::setOffset( const Eigen::Vector3d& offset ) {
  this->offset = -offset;
}

Eigen::Vector3d
FixedKinematicPrimitive::calculate( const Eigen::Vector3d& position, const Eigen::Vector3d& offset, const Eigen::Quaterniond& rotation ) {
  if( offset != Eigen::Vector3d( 0, 0, 0 ) ) {
    Eigen::Vector3d positionCorrection = rotation * offset;

    return Eigen::Vector3d(
      position.x() + positionCorrection.x(), position.y() + positionCorrection.y(), position.z() + positionCorrection.z() );
  } else {
    return position;
  }
}

void
FixedKinematicPrimitive::setPose( const Eigen::Vector3d&           position,
                                  const Eigen::Quaterniond&        rotation,
                                  const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoOrientation ) ) {
    orientationCalculated = rotation;
  } else {
    orientationCalculated = Eigen::Quaterniond( 0, 0, 0, 0 );
  }

  positionCalculated = calculate( position, offset, orientationCalculated );

  Q_EMIT poseChanged( positionCalculated, orientationCalculated, options );
}

QNEBlock*
FixedKinematicPrimitiveFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FixedKinematicPrimitive;
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Offset" ), QLatin1String( SLOT( setOffset( Eigen::Vector3d ) ) ) );
  b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
