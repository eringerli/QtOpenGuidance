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

#include "../kinematic/cgal.h"
#include "../kinematic/CgalWorker.h"
#include "StanleyGuidance.h"

void StanleyGuidance::setXteFrontWheels( double distance ) {
  if( !qIsInf( distance ) && velocity >= 0 ) {
    auto eulerFrontWheels = quaternionToEuler( orientationFrontWheels );
    double stanleyYawCompensation = /*normalizeAngleRadians*/( ( headingOfPathRadiansFrontWheels ) - eulerFrontWheels.z() );
    double stanleyXteCompensation = atan( ( stanleyGainKForwards * -distance ) / ( velocity + stanleyGainKSoftForwards ) );
    double stanleyYawDampening = /*normalizeAngleRadians*/( stanleyGainDampeningYawForwards *
        ( ( quaternionToEuler( this->orientation1AgoFrontWheels ).z() ) - eulerFrontWheels.z() ) -
        ( yawTrajectory1AgoFrontWheels - headingOfPathRadiansFrontWheels ) );
    double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringForwards * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
    double steerAngleRequested = qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

    if( steerAngleRequested >= maxSteeringAngle ) {
      steerAngleRequested = maxSteeringAngle;
    }

    if( steerAngleRequested <= -maxSteeringAngle ) {
      steerAngleRequested = -maxSteeringAngle;
    }

    qDebug() << Qt::fixed << Qt::forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadiansFrontWheels ) << normalizeAngleRadians( eulerFrontWheels.z() );

    emit steerAngleChanged( steerAngleRequested );
    yawTrajectory1AgoFrontWheels = headingOfPathRadiansFrontWheels;
  }
}

void StanleyGuidance::setXteRearWheels( double distance ) {
  if( !qIsInf( distance ) && velocity < 0 ) {
    auto eulerRearWheels = quaternionToEuler( orientationRearWheels );
    double stanleyYawCompensation = /*normalizeAngleRadians*/( ( headingOfPathRadiansRearWheels ) - eulerRearWheels.z() );
    double stanleyXteCompensation = atan( ( stanleyGainKReverse * -distance ) / ( velocity + -stanleyGainKSoftReverse ) );
    double stanleyYawDampening = /*normalizeAngleRadians*/( stanleyGainDampeningYawReverse *
        ( ( quaternionToEuler( this->orientation1AgoRearWheels ).z() ) - eulerRearWheels.z() ) -
        ( yawTrajectory1AgoRearWheels - headingOfPathRadiansRearWheels ) );
    double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringReverse * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
    double steerAngleRequested = -qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

    if( steerAngleRequested >= maxSteeringAngle ) {
      steerAngleRequested = maxSteeringAngle;
    }

    if( steerAngleRequested <= -maxSteeringAngle ) {
      steerAngleRequested = -maxSteeringAngle;
    }

    //        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadiansRearWheels ) << normalizeAngleRadians( qDegreesToRadians( orientationRearWheels.toEulerAngles().z() ) );

    emit steerAngleChanged( steerAngleRequested );
    yawTrajectory1AgoRearWheels = headingOfPathRadiansRearWheels;
  }
}
