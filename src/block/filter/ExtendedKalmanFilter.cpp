// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
ExtendedKalmanFilter::setPosition( const Eigen::Vector3d&, const CalculationOption::Options ) {
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
ExtendedKalmanFilter::setImuData( const double              dT,
                                  const Eigen::Quaterniond& orientation,
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
ExtendedKalmanFilter::setOrientation( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
  orientation = value;
}

void
ExtendedKalmanFilter::setOrientationCorrection( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
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

  b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( POSITION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity 3D" ), QLatin1String( SLOT( setVelocity3D( const Eigen::Vector3d& ) ) ) );
  b->addInputPort(
    QStringLiteral( "IMU Data" ),
    QLatin1String( SLOT( setImuData( const double, const Eigen::Quaterniond&, const Eigen::Vector3d&, const Eigen::Vector3d& ) ) ) );

  b->addInputPort( QStringLiteral( "Orientation Correction" ), QLatin1String( SLOT( setOrientationCorrection( ORIENTATION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
