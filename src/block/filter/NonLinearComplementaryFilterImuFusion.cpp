// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NonLinearComplementaryFilterImuFusion.h"

#include "Eigen/src/Geometry/Quaternion.h"
#include "helpers/anglesHelper.h"
#include "kinematic/CalculationOptions.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"
#include <cmath>

#include <attitude_estimator.h>
#include <memory>

#include <GeographicLib/MagneticModel.hpp>
#include <exception>

template< typename T >
constexpr T
sech( const T& z ) {
  return T( 1 ) / std::cosh( z );
}

template< typename T >
constexpr T
sec( const T& z ) {
  return T( 1 ) / std::cos( z );
}

NonLinearComplementaryFilterImuFusion::NonLinearComplementaryFilterImuFusion( const int idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  est = std::make_unique< stateestimation::AttitudeEstimator >();

  try {
    GeographicLib::MagneticModel mag( "wmm2020" );
    double                       lat = 7.5266, lon = 47.2197, h = 450, t = 2022; // Mt Everest
    double                       Bx, By, Bz;
    mag( t, lat, lon, h, Bx, By, Bz );
    std::cout << "Bx,By,Bz: " << Bx << " " << By << " " << Bz << " " << std::endl;
    //    Bx,By,Bz: 305.107 36295.8 -933.255

    double H, F, D, I;
    GeographicLib::MagneticModel::FieldComponents( Bx, By, Bz, H, F, D, I );
    std::cout << H << " " << F << " " << D << " " << I << std::endl;
  } catch( const std::exception& e ) {
    std::cerr << "Caught exception: " << e.what() << std::endl;
  }

  //  est->setMagCalib(
  //    0.68, -1.32, 0.0 ); // Recommended: Use if you want absolute yaw information as opposed to just relative yaw (Default: (1.0, 0.0,
  //    0.0))

  est->resetAll();
}

void
NonLinearComplementaryFilterImuFusion::setImuData( const double           dT,
                                                   const Eigen::Vector3d& accelerometerData,
                                                   const Eigen::Vector3d& gyroData,
                                                   const Eigen::Vector3d& magnetometerData ) {
  if( dT < 0.1 ) {
    QElapsedTimer timer;
    timer.start();

    auto gyroDataRad = degreesToRadians( gyroData );

    //    Eigen::Vector3d magnetometerData2 = Eigen::Vector3d( -magnetometerData.y(), magnetometerData.x(), magnetometerData.z() );
    //    Eigen::Vector3d magnetometerData2 = magnetometerData;
    //    Eigen::Vector3d magnetometerData2 = Eigen::Vector3d( 0, 0, 0 );
    est->update( dT,
                 gyroDataRad.x(),
                 gyroDataRad.y(),
                 gyroDataRad.z(),
                 accelerometerData.x(),
                 accelerometerData.y(),
                 accelerometerData.z(),
                 magnetometerData.x(),
                 magnetometerData.y(),
                 magnetometerData.z() );

    double q[4];
    double bias[3];
    double Kp;
    double Ti;
    double KpQuick;
    double TiQuick;

    est->getAttitude( q );
    est->getGyroBias( bias );
    est->getPIGains( Kp, Ti, KpQuick, TiQuick );

    //    std::cout << "My attitude is (quaternion): (" << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << ")" << std::endl;
    //    std::cout << "dT, gyr, acc, mag: " << std::fixed << std::setprecision( 7 ) << std::setw( 12 ) << std::showpos //
    //              << dT << " ("                                                                                       //
    //              << gyroData.x() << ", " << gyroData.y() << ", " << gyroData.z() << ") ("                            //
    //              << accelerometerData.x() << ", " << accelerometerData.y() << ", " << accelerometerData.z() << ") (" //
    //              << magnetometerData2.x() << ", " << magnetometerData2.y() << ", " << magnetometerData2.z() << ")" << std::endl;
    //    std::cout << "gyr bias: " << std::fixed << std::setprecision( 7 ) << std::setw( 12 ) << std::showpos //
    //              << bias[0] << ", " << bias[1] << ", " << bias[2] << ")" << std::endl;
    //    std::cout << "Kp: " << std::fixed << std::setprecision( 3 ) << std::setw( 6 ) << std::showpos //
    //              << Kp << ", Ti: " << Ti << ", KpQuick: " << KpQuick << ", TiQuick: " << TiQuick << std::endl;
    Q_EMIT orientationChanged( Eigen::Quaterniond( q[0], q[1], q[2], q[3] ), CalculationOption::None );

    double processingTime = double( timer.nsecsElapsed() ) / 1.e6;

    Q_EMIT processingTimeChanged( processingTime, CalculationOption::Option::None );
    Q_EMIT maxProcessingTimeChanged( dT * 1000., CalculationOption::Option::None );
  }
}

void
NonLinearComplementaryFilterImuFusion::setOrientationCorrection( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
  orientationCorrection = value;
}

void
NonLinearComplementaryFilterImuFusion::setKp( const double value, const CalculationOption::Options ) {
  kP = value;

  double KpTmp;
  double TiTmp;
  double KpQuick;
  double TiQuick;
  est->getPIGains( KpTmp, TiTmp, KpQuick, TiQuick );

  est->setPIGains( kP, tI, KpQuick, TiQuick );
}

void
NonLinearComplementaryFilterImuFusion::setTi( const double value, const CalculationOption::Options ) {
  tI = value;

  double KpTmp;
  double TiTmp;
  double KpQuick;
  double TiQuick;
  est->getPIGains( KpTmp, TiTmp, KpQuick, TiQuick );

  est->setPIGains( kP, tI, KpQuick, TiQuick );
}

void
NonLinearComplementaryFilterImuFusion::emitOrientationFromCCF() {
  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

void
NonLinearComplementaryFilterImuFusion::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

std::unique_ptr< BlockBase >
NonLinearComplementaryFilterImuFusionFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< NonLinearComplementaryFilterImuFusion >( idHint );

  obj->addInputPort( QStringLiteral( "IMU Data" ), obj.get(), QLatin1StringView( SLOT( setImuData( IMU_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Orientation Correction" ), obj.get(), QLatin1StringView( SLOT( setOrientationCorrection( ORIENTATION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Kp" ), obj.get(), QLatin1StringView( SLOT( setKp( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Ti (Ki=Kp/Ti" ), obj.get(), QLatin1StringView( SLOT( setTi( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Processing Time [ms]" ), obj.get(), QLatin1StringView( SIGNAL( processingTimeChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Max Processing Time [ms]" ), obj.get(), QLatin1StringView( SIGNAL( maxProcessingTimeChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
