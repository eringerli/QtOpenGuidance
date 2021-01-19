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

void TrailerKinematicPrimitive::setOffset( const Eigen::Vector3d& offset ) {
  fixedKinematic.setOffset( offset );
}

void TrailerKinematicPrimitive::setMaxJackknifeAngle( const double maxJackknifeAngle ) {
  maxJackknifeAngleRad = degreesToRadians( maxJackknifeAngle );
}

void TrailerKinematicPrimitive::setMaxAngle( const double maxAngle ) {
  maxAngleRad = degreesToRadians( maxAngle );
}

void TrailerKinematicPrimitive::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation, const PoseOption::Options& options ) {
  if( options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
    orientation = Eigen::Quaterniond();
  } else {
    auto taitBryan = quaternionToTaitBryan( orientation );
    double angleRad = std::atan2( position.y() - positionCalculated.y(),
                                  position.x() - positionCalculated.x() );

    Eigen::Quaterniond orientationTmp;
    orientationTmp = taitBryanToQuaternion( 0, getPitch( taitBryan ), getRoll( taitBryan ) )
                     * taitBryanToQuaternion( angleRad, 0, 0 );

    // the angle between tractor and trailer > maxAngleToTowingKinematic -> reset orientation to the one from the tractor
    double angleDifferenceRad = getYaw( quaternionToTaitBryan( rotation.inverse() * orientationTmp ) );

    if( std::abs( angleDifferenceRad ) < maxJackknifeAngleRad ) {
      // limit the angle to maxAngle
      if( std::abs( angleDifferenceRad ) > maxAngleRad ) {
        orientation = taitBryanToQuaternion( maxAngleRad * ( angleDifferenceRad > 0 ? 1 : -1 ), 0, 0 )
                      * taitBryanToQuaternion( 0, getPitch( taitBryan ), getRoll( taitBryan ) );
      } else {
        orientation = orientationTmp;
      }
    } else {
      orientation = rotation;
    }
  }

  fixedKinematic.setPose( position, orientation, options );
  positionCalculated = fixedKinematic.positionCalculated;

  Q_EMIT poseChanged( positionCalculated, orientation, options );
}

QNEBlock* TrailerKinematicPrimitiveFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TrailerKinematicPrimitive;
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Offset In to Out" ), QLatin1String( SLOT( setOffset( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( const double ) ) ) );
  b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  return b;
}
