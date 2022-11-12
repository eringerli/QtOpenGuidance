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

#include "ExtendedKalmanFilter.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include "filter/BicycleModel/ImuMeasurementModel.h"
#include "filter/BicycleModel/PositionMeasurementModel.h"
#include "filter/BicycleModel/SystemModel.h"

#include "kalman/ExtendedKalmanFilter.hpp"

ExtendedKalmanFilter::ExtendedKalmanFilter() {
  x.setZero();
  ekf.init( x );
  elapsedTimer.start();
}

void
ExtendedKalmanFilter::setPosition( const Eigen::Vector3d& position ) {
  predict();

  KinematicModel::PositionMeasurementVector< double > measurement;
  measurement << position.x(), position.y(), position.z(), velocity3D.x(), velocity3D.y(), velocity3D.z();

  ekf.update( positionMeasumentModel, measurement );

  emitPoseFromEKF();
  //      qDebug() << "Cycle Time ExtendedKalmanFilter:  " << timer.nsecsElapsed() <<
  //      "ns";
}

void
ExtendedKalmanFilter::setVelocity3D( const Eigen::Vector3d& velocity3D ) {
  this->velocity3D = velocity3D;
}

void
ExtendedKalmanFilter::setImuData( const Eigen::Quaterniond& orientation,
                                  const Eigen::Vector3d&    accelerometerData,
                                  const Eigen::Vector3d&    gyroData ) {
  predict();

  auto taitBryan = quaternionToTaitBryan( orientation );

  KinematicModel::ImuMeasurementVector< double > measurement;
  measurement << normalizeAngleRadians( taitBryan.x() ), normalizeAngleRadians( taitBryan.y() ), normalizeAngleRadians( taitBryan.z() ),
    gyroData.x(), gyroData.y(), gyroData.z(), accelerometerData.x(), accelerometerData.y(), accelerometerData.z();

  ekf.update( imuMeasumentModel, measurement );

  emitPoseFromEKF();
}

void
ExtendedKalmanFilter::setOrientation( const Eigen::Quaterniond& value ) {
  orientation = value;
}

void
ExtendedKalmanFilter::setOrientationCorrection( const Eigen::Quaterniond& value ) {
  orientationCorrection = value;
}

void
ExtendedKalmanFilter::predict() {
  constexpr double msPerS      = 1000;
  double           elapsedTime = double( elapsedTimer.restart() ) / msPerS;

  ekf.predict( systemModel, elapsedTime );
}

void
ExtendedKalmanFilter::emitPoseFromEKF() {
  auto x = ekf.getState();

  this->position    = x.head( 3 );
  this->orientation = taitBryanToQuaternion(
    x( KinematicModel::StateNames::Roll ), x( KinematicModel::StateNames::Pitch ), x( KinematicModel::StateNames::Yaw ) );

  Q_EMIT poseChanged( this->position, this->orientation, CalculationOption::None );
}

void
ExtendedKalmanFilter::emitConfigSignals() {
  Q_EMIT poseChanged( position, orientation, CalculationOption::None );
}

QNEBlock*
ExtendedKalmanFilterFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ExtendedKalmanFilter();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity 3D" ), QLatin1String( SLOT( setVelocity3D( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "IMU Data" ),
                   QLatin1String( SLOT( setImuData( const Eigen::Quaterniond&, const Eigen::Vector3d&, const Eigen::Vector3d& ) ) ) );

  b->addInputPort( QStringLiteral( "Orientation Correction" ),
                   QLatin1String( SLOT( setOrientationCorrection( const Eigen::Quaterniond& ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
