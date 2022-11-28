// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "TrailerKinematicPrimitive.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/anglesHelper.h"
#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include <iostream>

void
TrailerKinematicPrimitive::setOffset( const Eigen::Vector3d& offset ) {
  fixedKinematic.setOffset( offset );
}

void
TrailerKinematicPrimitive::setMaxJackknifeAngle( const double maxJackknifeAngle, CalculationOption::Options ) {
  maxJackknifeAngleRad = degreesToRadians( maxJackknifeAngle );
}

void
TrailerKinematicPrimitive::setMaxAngle( const double maxAngle, CalculationOption::Options ) {
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

QNEBlock*
TrailerKinematicPrimitiveFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TrailerKinematicPrimitive;
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Offset In to Out" ), QLatin1String( SLOT( setOffset( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Initial Mpc Pose Pivot" ), QLatin1String( SLOT( setPoseInitialMpcPivot( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
