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

#include <QObject>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../filter/BicycleModel/SystemModel.h"
#include "../filter/BicycleModel/ImuMeasurementModel.h"
#include "../filter/BicycleModel/PositionMeasurementModel.h"

#include "kalman/ExtendedKalmanFilter.hpp"

class ExtendedKalmanFilter : public BlockBase {
    Q_OBJECT

    template<typename T>
    using State = KinematicModel::State<T>;

  public:
    explicit ExtendedKalmanFilter()
      : BlockBase() {
      x.setZero();
      ekf.init( x );
      elapsedTimer.start();
    }

  public slots:
    void setPosition( const Eigen::Vector3d position ) {

      predict();

      KinematicModel::PositionMeasurementVector<double> measurement;
      measurement << position.x(), position.y(), position.z(),
                  velocity3D.x(), velocity3D.y(), velocity3D.z();

      ekf.update( positionMeasumentModel, measurement );

      emitPoseFromEKF();
//      qDebug() << "Cycle Time ExtendedKalmanFilter:  " << timer.nsecsElapsed() << "ns";
    }

    void setVelocity3D( const Eigen::Vector3d velocity3D ) {
      this->velocity3D = velocity3D;
    }

    void setImuData( const Eigen::Quaterniond orientation, const Eigen::Vector3d accelerometerData, const Eigen::Vector3d gyroData ) {
      predict();

      auto eulers = quaternionToEuler( orientation );

      KinematicModel::ImuMeasurementVector<double> measurement;
      measurement << normalizeAngleRadians( eulers.x() ), normalizeAngleRadians( eulers.y() ), normalizeAngleRadians( eulers.z() ),
                  gyroData.x(), gyroData.y(), gyroData.z(),
                  accelerometerData.x(), accelerometerData.y(), accelerometerData.z();

      ekf.update( imuMeasumentModel, measurement );

      emitPoseFromEKF();
    }

    void setOrientation( const Eigen::Quaterniond value ) {
      orientation = value;
    }

    void setOrientationCorrection( const Eigen::Quaterniond value ) {
      orientationCorrection = value;
    }

  private:
    void predict() {
      constexpr double msPerS = 1000;
      double elapsedTime = double ( elapsedTimer.restart() ) / msPerS;

      ekf.predict( systemModel, elapsedTime );
    }

    void emitPoseFromEKF() {
      auto x = ekf.getState();

      this->position = convertEigenVector3ToPoint3( x.head( 3 ) );
      this->orientation = eulerToQuaternion( x( KinematicModel::StateNames::Roll ),
                                             x( KinematicModel::StateNames::Pitch ),
                                             x( KinematicModel::StateNames::Yaw ) );

      emit poseChanged( this->position, this->orientation, PoseOption::NoOptions );
    }

  signals:
    void poseChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );

  public:
    virtual void emitConfigSignals() override {
      emit poseChanged( position, orientation, PoseOption::NoOptions );
    }

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    Eigen::Vector3d velocity3D = Eigen::Vector3d( 0, 0, 0 );

    Eigen::Quaterniond orientation = Eigen::Quaterniond();
    Eigen::Quaterniond orientationCorrection = Eigen::Quaterniond();

    State<double> x;
    Kalman::ExtendedKalmanFilter<State<double>> ekf;

    KinematicModel::TractorImuGpsFusionModel<State> systemModel;
    KinematicModel::PositionMeasurement<double, State, KinematicModel::PositionMeasurementVector> positionMeasumentModel;
    KinematicModel::ImuMeasurement<double, State, KinematicModel::ImuMeasurementVector> imuMeasumentModel;

    QElapsedTimer elapsedTimer;
};

class ExtendedKalmanFilterFactory : public BlockFactory {
    Q_OBJECT

  public:
    ExtendedKalmanFilterFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "ExtendedKalmanFilter" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ExtendedKalmanFilter();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( const Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond ) ) ) );
      b->addInputPort( QStringLiteral( "Velocity 3D" ), QLatin1String( SLOT( setVelocity3D( const Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "IMU Data" ), QLatin1String( SLOT( setImuData( const Eigen::Quaterniond, const Eigen::Vector3d, const Eigen::Vector3d ) ) ) );

      b->addInputPort( QStringLiteral( "Orientation Correction" ), QLatin1String( SLOT( setOrientationCorrection( const Eigen::Quaterniond ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }
};
