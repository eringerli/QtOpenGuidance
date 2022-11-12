// Copyright( C ) 2022 Christian Riggenbach
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

#include "SideslipAnglesEstimator.h"

#include "Eigen/src/Core/Matrix.h"
#include "qneblock.h"
#include "qneport.h"

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"
#include <memory>

void
SideslipAnglesEstimator::setSteeringAngle( const double steeringAngle, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->steeringAngle = steeringAngle;
  }
}

void
SideslipAnglesEstimator::setPoseRearWheels( const Eigen::Vector3d&           position,
                                            const Eigen::Quaterniond&        orientation,
                                            const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->positionRearWheels    = position;
    this->orientationRearWheels = orientation;
  }
}

void
SideslipAnglesEstimator::setHeadingOfPathRearWheels( const double headingOfPath, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->headingOfPathRadiansRearWheels = degreesToRadians( headingOfPath );
  }
}

void
SideslipAnglesEstimator::setXteRearWheels( double distance, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    if( !qIsInf( distance ) && velocity > 0.5 ) {
      auto taitBryanRearWheels = quaternionToTaitBryan( orientationRearWheels );

      Eigen::Vector2d Xmeasure;
      {
        Xmeasure( ( int )SideslipAnglesObserver::StateVector::Theta ) =
          normalizeAngleRadians( ( headingOfPathRadiansRearWheels )-getYaw( taitBryanRearWheels ) );
        Xmeasure( ( int )SideslipAnglesObserver::StateVector::y ) = distance;
      }

      Eigen::Matrix2d G;
      {
        G( 0, 0 ) = -2.8;
        G( 0, 1 ) = 0;
        G( 1, 0 ) = 0;
        G( 1, 1 ) = -0.8;
      }

      auto sideslipAnglesObserverNew =
        std::make_unique< SideslipAnglesObserver >( *sideslipAnglesObserver, Xmeasure, G, velocity, steeringAngle, curvature, 1. / 20. );

      Q_EMIT sideslipAngleFrontChanged(
        radiansToDegrees( sideslipAnglesObserverNew->u( ( int )SideslipAnglesObserver::InputVector::Beta_F_P ) ),
        CalculationOption::Option::None );
      Q_EMIT sideslipAngleRearChanged(
        radiansToDegrees( sideslipAnglesObserverNew->u( ( int )SideslipAnglesObserver::InputVector::Beta_R_P ) ),
        CalculationOption::Option::None );

      std::cout << "SideslipAnglesEstimator: Xmeasure: " << printVector( Xmeasure ) << ", curvature: " << curvature
                << ", velocity: " << velocity << ", steeringAngle: " << steeringAngle << std::endl;

      sideslipAnglesObserver = std::move( sideslipAnglesObserverNew );
    }
  }
}

void
SideslipAnglesEstimator::setCurvature( const double curvature, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->curvature = curvature;
  }
}

void
SideslipAnglesEstimator::setVelocity( const double velocity, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoControl ) ) {
    this->velocity = velocity;
  }
}

void
SideslipAnglesEstimator::emitConfigSignals() {
  Q_EMIT sideslipAngleFrontChanged( 0, CalculationOption::Option::None );
  Q_EMIT sideslipAngleRearChanged( 0, CalculationOption::Option::None );
}

QNEBlock*
SideslipAnglesEstimatorFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SideslipAnglesEstimator();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose Rear Wheels" ), QLatin1String( SLOT( setPoseRearWheels( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "XTE Rear Wheels" ), QLatin1String( SLOT( setXteRearWheels( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ),
                   QLatin1String( SLOT( setHeadingOfPathRearWheels( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Curvature of Path" ), QLatin1String( SLOT( setCurvature( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Sideslip Angle Front" ), QLatin1String( SIGNAL( sideslipAngleFrontChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Sideslip Angle Rear" ), QLatin1String( SIGNAL( sideslipAngleRearChanged( NUMBER_SIGNATURE ) ) ) );

  return b;
}
