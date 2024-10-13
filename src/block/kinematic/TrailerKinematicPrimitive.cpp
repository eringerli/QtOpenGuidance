// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TrailerKinematicPrimitive.h"

#include "helpers/anglesHelper.h"
#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include <iostream>

void
TrailerKinematicPrimitive::setOffset( const Eigen::Vector3d& offset, const CalculationOption::Options ) {
  fixedKinematic.setOffset( offset );
}

void
TrailerKinematicPrimitive::setMaxJackknifeAngle( const double maxJackknifeAngle, const CalculationOption::Options ) {
  maxJackknifeAngleRad = degreesToRadians( maxJackknifeAngle );
}

void
TrailerKinematicPrimitive::setMaxAngle( const double maxAngle, const CalculationOption::Options ) {
  maxAngleRad = degreesToRadians( maxAngle );
}

void
TrailerKinematicPrimitive::setPose( const Eigen::Vector3d&           position,
                                    const Eigen::Quaterniond&        orientation,
                                    const CalculationOption::Options options ) {
  if( options.testFlag( CalculationOption::RestoreStateToBeforeMpcCalculation ) ) {
    positionCalculated                   = positionMpcInitialPivot;
    orientationCalculated                = orientationMpcInitialPivot;
    lastPosition                         = positionMpcInitialPivot;
    fixedKinematic.positionCalculated    = positionMpcInitialPivot;
    fixedKinematic.orientationCalculated = orientationMpcInitialPivot;
    Q_EMIT poseChanged( positionCalculated, orientationCalculated, options );
    return;
  }

  if( options.testFlag( CalculationOption::NoOrientation ) ) {
    fixedKinematic.setPose( position, Eigen::Quaterniond( 0, 0, 0, 0 ), options );
    Q_EMIT poseChanged( fixedKinematic.positionCalculated, Eigen::Quaterniond( 0, 0, 0, 0 ), options );
  } else {
    auto absoluteOrientationOfPlane = getAbsoluteOrientation( orientation );
    auto absoluteHeading            = ( orientationCalculated.inverse() * absoluteOrientationOfPlane ).inverse();

    fixedKinematic.setPose( lastPosition, absoluteHeading, options );

    double angleRad =
      std::atan2( position.y() - fixedKinematic.positionCalculated.y(), position.x() - fixedKinematic.positionCalculated.x() );

    auto orientationTmp = taitBryanToQuaternion( angleRad, 0, 0 );

    Eigen::Quaterniond heading;
    if( !options.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
      // the angle between tractor and trailer > maxAngleToTowingKinematic -> reset
      // orientation to the one from the tractor
      double angleDifferenceRad = getYaw( quaternionToTaitBryan( orientation.inverse() * orientationTmp ) );
      if( std::abs( angleDifferenceRad ) < maxJackknifeAngleRad ) {
        // limit the angle to maxAngle
        if( std::abs( angleDifferenceRad ) > maxAngleRad ) {
          heading = taitBryanToQuaternion( std::copysign( maxAngleRad, angleDifferenceRad ), 0, 0 );
        } else {
          heading = orientationTmp;
        }
      } else {
        heading = orientation;
      }
    } else {
      heading = orientationTmp;
    }

    orientationCalculated = absoluteOrientationOfPlane * heading;

    fixedKinematic.setPose( position, orientationCalculated, options );
    positionCalculated = fixedKinematic.positionCalculated;

    lastPosition = position;

    //    if( !options.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
    //      positionCalculatedBeforeMpcCalculation = positionCalculated;
    //      orientationBeforeMpcCalculation        = orientation;
    //      lastPositionBeforeMpcCalculation       = lastPosition;
    //    }

    Q_EMIT poseChanged( positionCalculated, orientationCalculated, options );
  }
}

void
TrailerKinematicPrimitive::setPoseInitialMpcPivot( const Eigen::Vector3d&           position,
                                                   const Eigen::Quaterniond&        orientation,
                                                   const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::CalculateMpcUpdate ) ) {
    positionMpcInitialPivot    = position;
    orientationMpcInitialPivot = orientation;
  }
}

std::unique_ptr< BlockBase >
TrailerKinematicPrimitiveFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< TrailerKinematicPrimitive >( idHint );

  obj->addInputPort( QStringLiteral( "Offset In to Out" ), obj.get(), QLatin1StringView( SLOT( setOffset( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "MaxJackknifeAngle" ), obj.get(), QLatin1StringView( SLOT( setMaxJackknifeAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "MaxAngle" ), obj.get(), QLatin1StringView( SLOT( setMaxAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose In" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Initial Mpc Pose Pivot" ), obj.get(), QLatin1StringView( SLOT( setPoseInitialMpcPivot( POSE_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Pose Out" ), obj.get(), QLatin1StringView( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
