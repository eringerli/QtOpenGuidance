// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FixedKinematicPrimitive.h"

#include "kinematic/CalculationOptions.h"

void
FixedKinematicPrimitive::setOffset( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
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

std::unique_ptr< BlockBase >
FixedKinematicPrimitiveFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< FixedKinematicPrimitive >( idHint );

  obj->addInputPort( QStringLiteral( "Offset" ), obj.get(), QLatin1StringView( SLOT( setOffset( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose In" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Pose Out" ), obj.get(), QLatin1StringView( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
