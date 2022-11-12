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

#pragma once

#include <QElapsedTimer>
#include <QObject>

#include "BlockBase.h"

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
  void setPosition( const Eigen::Vector3d& position );

  void setVelocity3D( const Eigen::Vector3d& velocity3D );

  void setImuData( const Eigen::Quaterniond& orientation, const Eigen::Vector3d& accelerometerData, const Eigen::Vector3d& gyroData );

  void setOrientation( const Eigen::Quaterniond& value );

  void setOrientationCorrection( const Eigen::Quaterniond& value );

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
  ExtendedKalmanFilterFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ExtendedKalmanFilter" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Extended Kalman Filter" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Base Blocks" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
