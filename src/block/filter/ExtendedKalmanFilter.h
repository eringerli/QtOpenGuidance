// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QObject>

#include "../BlockBase.h"

#include "helpers/eigenHelper.h"

#include "filter/BicycleModel/ImuMeasurementModel.h"
#include "filter/BicycleModel/PositionMeasurementModel.h"
#include "filter/BicycleModel/SystemModel.h"

#include "kalman/ExtendedKalmanFilter.hpp"

class ExtendedKalmanFilter : public BlockBase {
  Q_OBJECT

  template< typename T >
  using State = KinematicModel::State< T >;

public:
  explicit ExtendedKalmanFilter();

public Q_SLOTS:
  void setPosition( VECTOR_SIGNATURE_SLOT );

  void setVelocity3D( VECTOR_SIGNATURE_SLOT );

  void setImuData( IMU_SIGNATURE_SLOT );

  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

  void setOrientationCorrection( ORIENTATION_SIGNATURE_SLOT );

private:
  void predict();

  void emitPoseFromEKF();

Q_SIGNALS:
  void poseChanged( POSE_SIGNATURE_SIGNAL );

public:
  virtual void emitConfigSignals() override;

public:
  Eigen::Vector3d position   = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Vector3d velocity3D = Eigen::Vector3d( 0, 0, 0 );

  Eigen::Quaterniond orientation           = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Quaterniond orientationCorrection = Eigen::Quaterniond( 0, 0, 0, 0 );

  State< double >                                 x;
  Kalman::ExtendedKalmanFilter< State< double > > ekf;

  KinematicModel::TractorImuGpsFusionModel< State >                                               systemModel;
  KinematicModel::PositionMeasurement< double, State, KinematicModel::PositionMeasurementVector > positionMeasumentModel;
  KinematicModel::ImuMeasurement< double, State, KinematicModel::ImuMeasurementVector >           imuMeasumentModel;

  QElapsedTimer elapsedTimer;
};

class ExtendedKalmanFilterFactory : public BlockFactory {
  Q_OBJECT

public:
  ExtendedKalmanFilterFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ExtendedKalmanFilter" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Extended Kalman Filter" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Filter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
