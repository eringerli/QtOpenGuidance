// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StanleyGuidance.h"

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"
#include <algorithm>

void
StanleyGuidance::setSteeringAngle( const double steeringAngle, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    steeringAngle2Ago   = steeringAngle1Ago;
    steeringAngle1Ago   = this->steeringAngle;
    this->steeringAngle = steeringAngle;
  }
}

void
StanleyGuidance::setPoseFrontWheels( const Eigen::Vector3d&           position,
                                     const Eigen::Quaterniond&        orientation,
                                     const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoControl ) ) {
    this->positionFrontWheels        = position;
    this->orientation1AgoFrontWheels = this->orientationFrontWheels;
    this->orientationFrontWheels     = orientation;
  }
}

void
StanleyGuidance::setPoseRearWheels( const Eigen::Vector3d&           position,
                                    const Eigen::Quaterniond&        orientation,
                                    const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoControl ) ) {
    this->positionRearWheels        = position;
    this->orientation1AgoRearWheels = this->orientationRearWheels;
    this->orientationRearWheels     = orientation;
  }
}

void
StanleyGuidance::setHeadingOfPathFrontWheels( const double headingOfPath, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->headingOfPathRadiansFrontWheels = degreesToRadians( headingOfPath );
  }
}

void
StanleyGuidance::setHeadingOfPathRearWheels( const double headingOfPath, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->headingOfPathRadiansRearWheels = degreesToRadians( headingOfPath );
  }
}

void
StanleyGuidance::setXteFrontWheels( double distance, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoControl ) ) {
    if( !std::isinf( distance ) && velocity >= 0 ) {
      auto   taitBryanFrontWheels   = quaternionToTaitBryan( orientationFrontWheels );
      double stanleyYawCompensation = normalizeAngleRadians( ( headingOfPathRadiansFrontWheels )-getYaw( taitBryanFrontWheels ) );
      double stanleyXteCompensation = atan( ( stanleyGainKForwards * -distance ) / ( velocity + stanleyGainKSoftForwards ) );
      double stanleyYawDampening =
        normalizeAngleRadians( stanleyGainDampeningYawForwards *
                                 ( getYaw( quaternionToTaitBryan( this->orientation1AgoFrontWheels ) ) - getYaw( taitBryanFrontWheels ) ) -
                               ( yawTrajectory1AgoFrontWheels - headingOfPathRadiansFrontWheels ) );
      double stanleySteeringDampening =
        normalizeAngleRadians( stanleyGainDampeningSteeringForwards * degreesToRadians( steeringAngle1Ago - steeringAngle ) );
      double steerAngleRequested      = radiansToDegrees(
      normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation/* +
                             stanleyYawDampening + stanleySteeringDampening*/ ) );

      //    std::cout << "setXteFrontWheels: " << taitBryanFrontWheels << ", "
      //              << stanleyYawDampening << ", " << stanleySteeringDampening <<
      //              std::endl;

      steerAngleRequested = std::clamp( steerAngleRequested, -maxSteeringAngle, maxSteeringAngle );

      //    qDebug() << Qt::fixed << Qt::forcesign << qSetRealNumberPrecision( 4 ) <<
      //    stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening <<
      //    stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians(
      //    headingOfPathRadiansFrontWheels ) << normalizeAngleRadians(
      //    taitBryanFrontWheels.z() );

      Q_EMIT steerAngleChanged( steerAngleRequested, CalculationOption::Option::None );
      yawTrajectory1AgoFrontWheels = headingOfPathRadiansFrontWheels;
    }
  }
}

void
StanleyGuidance::setXteRearWheels( double distance, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoControl ) ) {
    if( !std::isinf( distance ) && velocity < 0 ) {
      auto   taitBryanRearWheels = quaternionToTaitBryan( orientationRearWheels );
      double stanleyYawCompensation =
        /*normalizeAngleRadians*/ ( ( headingOfPathRadiansRearWheels )-getYaw( taitBryanRearWheels ) );
      double stanleyXteCompensation = atan( ( stanleyGainKReverse * -distance ) / ( velocity + -stanleyGainKSoftReverse ) );
      double stanleyYawDampening =
        /*normalizeAngleRadians*/ ( stanleyGainDampeningYawReverse * ( getYaw( quaternionToTaitBryan( this->orientation1AgoRearWheels ) ) -
                                                                       getYaw( taitBryanRearWheels ) ) -
                                    ( yawTrajectory1AgoRearWheels - headingOfPathRadiansRearWheels ) );
      double stanleySteeringDampening =
        /*normalizeAngle*/ ( stanleyGainDampeningSteeringReverse * degreesToRadians( steeringAngle1Ago - steeringAngle ) );
      double steerAngleRequested = -radiansToDegrees(
        normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

      steerAngleRequested = std::clamp( steerAngleRequested, -maxSteeringAngle, maxSteeringAngle );

      //        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) <<
      //        stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening <<
      //        stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians(
      //        headingOfPathRadiansRearWheels ) << normalizeAngleRadians(
      //        degreesToRadians( orientationRearWheels.toEulerAngles().z() ) );

      if( !std::isinf( steerAngleRequested ) ) {
        Q_EMIT steerAngleChanged( steerAngleRequested, CalculationOption::Option::None );
        yawTrajectory1AgoRearWheels = headingOfPathRadiansRearWheels;
      }
    }
  }
}

void
StanleyGuidance::setStanleyGainKForwards( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainKForwards = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainKSoftForwards( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainKSoftForwards = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainDampeningYawForwards( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainDampeningYawForwards = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainDampeningSteeringForwards( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainDampeningSteeringForwards = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainKReverse( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainKReverse = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainKSoftReverse( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainKSoftReverse = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainDampeningYawReverse( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainDampeningYawReverse = stanleyGain;
  }
}

void
StanleyGuidance::setStanleyGainDampeningSteeringReverse( const double stanleyGain, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->stanleyGainDampeningSteeringReverse = stanleyGain;
  }
}

void
StanleyGuidance::setVelocity( const double velocity, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->velocity = velocity;
  }
}

void
StanleyGuidance::setMaxSteeringAngle( const double maxSteeringAngle, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->maxSteeringAngle = maxSteeringAngle;
  }
}

void
StanleyGuidance::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT steerAngleChanged( 0, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
StanleyGuidanceFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< StanleyGuidance >( idHint );

  obj->addInputPort( QStringLiteral( "Pose Front Wheels" ), obj.get(), QLatin1StringView( SLOT( setPoseFrontWheels( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose Rear Wheels" ), obj.get(), QLatin1StringView( SLOT( setPoseRearWheels( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "Velocity" ), obj.get(), QLatin1StringView( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "XTE Front Wheels" ), obj.get(), QLatin1StringView( SLOT( setXteFrontWheels( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Heading of Path Front Wheels" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setHeadingOfPathFrontWheels( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "XTE Rear Wheels" ), obj.get(), QLatin1StringView( SLOT( setXteRearWheels( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setHeadingOfPathRearWheels( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort(
    QStringLiteral( "Stanley Gain K Forwards" ), obj.get(), QLatin1StringView( SLOT( setStanleyGainKForwards( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain K Softening Forwards" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainKSoftForwards( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Forwards" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainDampeningYawForwards( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Forwards" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainDampeningSteeringForwards( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort(
    QStringLiteral( "Stanley Gain K Reverse" ), obj.get(), QLatin1StringView( SLOT( setStanleyGainKReverse( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain K Softening Reverse" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainKSoftReverse( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Reverse" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainDampeningYawReverse( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Reverse" ),
                     obj.get(),
                     QLatin1StringView( SLOT( setStanleyGainDampeningSteeringReverse( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort(
    QStringLiteral( "Max Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setMaxSteeringAngle( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Steer Angle" ), obj.get(), QLatin1StringView( SIGNAL( steerAngleChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
