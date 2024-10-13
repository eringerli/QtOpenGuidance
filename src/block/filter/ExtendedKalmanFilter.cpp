// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ExtendedKalmanFilter.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include "filter/BicycleModel/ImuMeasurementModel.h"
#include "filter/BicycleModel/PositionMeasurementModel.h"
#include "filter/BicycleModel/SystemModel.h"

#include "kalman/ExtendedKalmanFilter.hpp"

ExtendedKalmanFilter::ExtendedKalmanFilter( const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
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
ExtendedKalmanFilter::setVelocity3D( const Eigen::Vector3d& velocity3D, const CalculationOption::Options ) {
  this->velocity3D = velocity3D;
}

void
ExtendedKalmanFilter::setImuData( const double           dT,
                                  const Eigen::Vector3d& accelerometerData,
                                  const Eigen::Vector3d& gyroData,
                                  const Eigen::Vector3d& magnetometerData ) {
  //  predict();

  //  auto taitBryan = quaternionToTaitBryan( orientation );

  //  KinematicModel::ImuMeasurementVector< double > measurement;
  //  measurement << normalizeAngleRadians( taitBryan.x() ), normalizeAngleRadians( taitBryan.y() ), normalizeAngleRadians( taitBryan.z() ),
  //    gyroData.x(), gyroData.y(), gyroData.z(), accelerometerData.x(), accelerometerData.y(), accelerometerData.z();

  //  ekf.update( imuMeasumentModel, measurement );

  //  emitPoseFromEKF();
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
  BlockBase::emitConfigSignals();
  Q_EMIT poseChanged( position, orientation, CalculationOption::None );
}

std::unique_ptr< BlockBase >
ExtendedKalmanFilterFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ExtendedKalmanFilter >( idHint );

  obj->addInputPort( QStringLiteral( "Position" ), obj.get(), QLatin1StringView( SLOT( setPosition( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Velocity 3D" ), obj.get(), QLatin1StringView( SLOT( setVelocity3D( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "IMU Data" ), obj.get(), QLatin1StringView( SLOT( setImuData( IMU_SIGNATURE ) ) ) );

  obj->addInputPort(
    QStringLiteral( "Orientation Correction" ), obj.get(), QLatin1StringView( SLOT( setOrientationCorrection( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
