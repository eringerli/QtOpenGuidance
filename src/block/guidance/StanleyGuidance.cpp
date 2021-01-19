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

#include "StanleyGuidance.h"

#include "qneblock.h"
#include "qneport.h"

#include "kinematic/cgal.h"
#include "kinematic/CgalWorker.h"

void StanleyGuidance::setSteeringAngle( const double steeringAngle ) {
  steeringAngle2Ago = steeringAngle1Ago;
  steeringAngle1Ago = this->steeringAngle;
  this->steeringAngle = steeringAngle;
}

void StanleyGuidance::setPoseFrontWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    this->positionFrontWheels = position;
    this->orientation1AgoFrontWheels = this->orientationFrontWheels;
    this->orientationFrontWheels = orientation;
  }
}

void StanleyGuidance::setPoseRearWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    this->positionRearWheels = position;
    this->orientation1AgoRearWheels = this->orientationRearWheels;
    this->orientationRearWheels = orientation;
  }
}

void StanleyGuidance::setHeadingOfPathFrontWheels( const double headingOfPath ) {
  this->headingOfPathRadiansFrontWheels = degreesToRadians( headingOfPath );
}

void StanleyGuidance::setHeadingOfPathRearWheels( const double headingOfPath ) {
  this->headingOfPathRadiansRearWheels = degreesToRadians( headingOfPath );
}

void StanleyGuidance::setXteFrontWheels( double distance ) {
  if( !qIsInf( distance ) && velocity >= 0 ) {
    auto taitBryanFrontWheels = quaternionToTaitBryan( orientationFrontWheels );
    double stanleyYawCompensation = /*normalizeAngleRadians*/( ( headingOfPathRadiansFrontWheels ) - getYaw( taitBryanFrontWheels ) );
    double stanleyXteCompensation = atan( ( stanleyGainKForwards * -distance ) / ( velocity + stanleyGainKSoftForwards ) );
    double stanleyYawDampening = /*normalizeAngleRadians*/( stanleyGainDampeningYawForwards *
        ( getYaw( quaternionToTaitBryan( this->orientation1AgoFrontWheels ) ) - getYaw( taitBryanFrontWheels ) ) -
        ( yawTrajectory1AgoFrontWheels - headingOfPathRadiansFrontWheels ) );
    double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringForwards * degreesToRadians( steeringAngle1Ago - steeringAngle ) );
    double steerAngleRequested = radiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

    if( steerAngleRequested >= maxSteeringAngle ) {
      steerAngleRequested = maxSteeringAngle;
    }

    if( steerAngleRequested <= -maxSteeringAngle ) {
      steerAngleRequested = -maxSteeringAngle;
    }

//    qDebug() << Qt::fixed << Qt::forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadiansFrontWheels ) << normalizeAngleRadians( taitBryanFrontWheels.z() );

    Q_EMIT steerAngleChanged( steerAngleRequested );
    yawTrajectory1AgoFrontWheels = headingOfPathRadiansFrontWheels;
  }
}

void StanleyGuidance::setXteRearWheels( double distance ) {
  if( !qIsInf( distance ) && velocity < 0 ) {
    auto taitBryanRearWheels = quaternionToTaitBryan( orientationRearWheels );
    double stanleyYawCompensation = /*normalizeAngleRadians*/( ( headingOfPathRadiansRearWheels ) - getYaw( taitBryanRearWheels ) );
    double stanleyXteCompensation = atan( ( stanleyGainKReverse * -distance ) / ( velocity + -stanleyGainKSoftReverse ) );
    double stanleyYawDampening = /*normalizeAngleRadians*/( stanleyGainDampeningYawReverse *
        ( getYaw( quaternionToTaitBryan( this->orientation1AgoRearWheels ) ) - getYaw( taitBryanRearWheels ) ) -
        ( yawTrajectory1AgoRearWheels - headingOfPathRadiansRearWheels ) );
    double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringReverse * degreesToRadians( steeringAngle1Ago - steeringAngle ) );
    double steerAngleRequested = -radiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

    if( steerAngleRequested >= maxSteeringAngle ) {
      steerAngleRequested = maxSteeringAngle;
    }

    if( steerAngleRequested <= -maxSteeringAngle ) {
      steerAngleRequested = -maxSteeringAngle;
    }

    //        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadiansRearWheels ) << normalizeAngleRadians( degreesToRadians( orientationRearWheels.toEulerAngles().z() ) );

    Q_EMIT steerAngleChanged( steerAngleRequested );
    yawTrajectory1AgoRearWheels = headingOfPathRadiansRearWheels;
  }
}

void StanleyGuidance::setStanleyGainKForwards( const double stanleyGain ) {
  this->stanleyGainKForwards = stanleyGain;
}

void StanleyGuidance::setStanleyGainKSoftForwards( const double stanleyGain ) {
  this->stanleyGainKSoftForwards = stanleyGain;
}

void StanleyGuidance::setStanleyGainDampeningYawForwards( const double stanleyGain ) {
  this->stanleyGainDampeningYawForwards = stanleyGain;
}

void StanleyGuidance::setStanleyGainDampeningSteeringForwards( const double stanleyGain ) {
  this->stanleyGainDampeningSteeringForwards = stanleyGain;
}

void StanleyGuidance::setStanleyGainKReverse( const double stanleyGain ) {
  this->stanleyGainKReverse = stanleyGain;
}

void StanleyGuidance::setStanleyGainKSoftReverse( const double stanleyGain ) {
  this->stanleyGainKSoftReverse = stanleyGain;
}

void StanleyGuidance::setStanleyGainDampeningYawReverse( const double stanleyGain ) {
  this->stanleyGainDampeningYawReverse = stanleyGain;
}

void StanleyGuidance::setStanleyGainDampeningSteeringReverse( const double stanleyGain ) {
  this->stanleyGainDampeningSteeringReverse = stanleyGain;
}

void StanleyGuidance::setVelocity( const double velocity ) {
  this->velocity = velocity;
}

void StanleyGuidance::setMaxSteeringAngle( const double maxSteeringAngle ) {
  this->maxSteeringAngle = maxSteeringAngle;
}

void StanleyGuidance::emitConfigSignals() {
  Q_EMIT steerAngleChanged( 0 );
}

QNEBlock* StanleyGuidanceFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new StanleyGuidance();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Pose Front Wheels" ), QLatin1String( SLOT( setPoseFrontWheels( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Rear Wheels" ), QLatin1String( SLOT( setPoseRearWheels( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( const double ) ) ) );

  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( const double ) ) ) );

  b->addInputPort( QStringLiteral( "XTE Front Wheels" ), QLatin1String( SLOT( setXteFrontWheels( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Heading of Path Front Wheels" ), QLatin1String( SLOT( setHeadingOfPathFrontWheels( const double ) ) ) );
  b->addInputPort( QStringLiteral( "XTE Rear Wheels" ), QLatin1String( SLOT( setXteRearWheels( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ), QLatin1String( SLOT( setHeadingOfPathRearWheels( const double ) ) ) );

  b->addInputPort( QStringLiteral( "Stanley Gain K Forwards" ), QLatin1String( SLOT( setStanleyGainKForwards( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain K Softening Forwards" ), QLatin1String( SLOT( setStanleyGainKSoftForwards( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningYawForwards( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringForwards( const double ) ) ) );

  b->addInputPort( QStringLiteral( "Stanley Gain K Reverse" ), QLatin1String( SLOT( setStanleyGainKReverse( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain K Softening Reverse" ), QLatin1String( SLOT( setStanleyGainKSoftReverse( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningYawReverse( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringReverse( const double ) ) ) );

  b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( const double ) ) ) );

  b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( const double ) ) ) );

  return b;
}
