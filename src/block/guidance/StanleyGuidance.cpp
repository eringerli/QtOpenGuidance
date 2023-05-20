// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StanleyGuidance.h"

#include "qneblock.h"
#include "qneport.h"

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
  Q_EMIT steerAngleChanged( 0, CalculationOption::Option::None );
}

QNEBlock*
StanleyGuidanceFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new StanleyGuidance();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose Front Wheels" ), QLatin1String( SLOT( setPoseFrontWheels( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Rear Wheels" ), QLatin1String( SLOT( setPoseRearWheels( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "XTE Front Wheels" ), QLatin1String( SLOT( setXteFrontWheels( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Heading of Path Front Wheels" ),
                   QLatin1String( SLOT( setHeadingOfPathFrontWheels( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "XTE Rear Wheels" ), QLatin1String( SLOT( setXteRearWheels( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ),
                   QLatin1String( SLOT( setHeadingOfPathRearWheels( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Stanley Gain K Forwards" ), QLatin1String( SLOT( setStanleyGainKForwards( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain K Softening Forwards" ),
                   QLatin1String( SLOT( setStanleyGainKSoftForwards( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Forwards" ),
                   QLatin1String( SLOT( setStanleyGainDampeningYawForwards( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Forwards" ),
                   QLatin1String( SLOT( setStanleyGainDampeningSteeringForwards( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Stanley Gain K Reverse" ), QLatin1String( SLOT( setStanleyGainKReverse( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain K Softening Reverse" ),
                   QLatin1String( SLOT( setStanleyGainKSoftReverse( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Reverse" ),
                   QLatin1String( SLOT( setStanleyGainDampeningYawReverse( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Reverse" ),
                   QLatin1String( SLOT( setStanleyGainDampeningSteeringReverse( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( NUMBER_SIGNATURE ) ) ) );

  return b;
}
