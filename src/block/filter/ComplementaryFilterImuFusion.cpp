// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComplementaryFilterImuFusion.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"
#include <cmath>

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

ComplementaryFilterImuFusion::ComplementaryFilterImuFusion( const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  est = std::make_unique< imu_tools::ComplementaryFilter >();
}

void
ComplementaryFilterImuFusion::setImuData( const double           dT,
                                          const Eigen::Vector3d& accelerometerData,
                                          const Eigen::Vector3d& gyroData,
                                          const Eigen::Vector3d& magnetometerData ) {
  if( dT < 0.1 ) {
    QElapsedTimer timer;
    timer.start();

    auto gyroDataRad = degreesToRadians( gyroData );

    auto accelerometerData2 = accelerometerData.normalized();

    //    Eigen::Vector3d magnetometerData2 = Eigen::Vector3d( -magnetometerData.y(), magnetometerData.x(), magnetometerData.z() );

    if( magnetometerData == Eigen::Vector3d( 0, 0, 0 ) ) {
      est->update(
        accelerometerData2.x(), accelerometerData2.y(), accelerometerData2.z(), gyroDataRad.x(), gyroDataRad.y(), gyroDataRad.z(), dT );
    } else {
      est->update( accelerometerData2.x(),
                   accelerometerData2.y(),
                   accelerometerData2.z(),
                   gyroDataRad.x(),
                   gyroDataRad.y(),
                   gyroDataRad.z(),
                   magnetometerData.x(),
                   magnetometerData.y(),
                   magnetometerData.z(),
                   dT );
    }

    double q[4];
    est->getOrientation( q[0], q[1], q[2], q[3] );

    //    std::cout << "My attitude is (quaternion): (" << q[0] << "," << q[1] << "," << q[2] << "," << q[3] << ")" << std::endl;
    //    std::cout << "dT, gyr, acc, mag: " << std::fixed << std::setprecision( 7 ) << std::setw( 12 ) << std::showpos //
    //              << dT << " ("                                                                                       //
    //              << gyroData.x() << ", " << gyroData.y() << ", " << gyroData.z() << ") ("                            //
    //              << accelerometerData.x() << ", " << accelerometerData.y() << ", " << accelerometerData.z() << ") (" //
    //              << magnetometerData2.x() << ", " << magnetometerData2.y() << ", " << magnetometerData2.z() << ")" << std::endl;
    //    std::cout << "gyr bias: " << std::fixed << std::setprecision( 7 ) << std::setw( 12 ) << std::showpos //
    //              << est->getAngularVelocityBiasX() << ", " << est->getAngularVelocityBiasY() << ", " << est->getAngularVelocityBiasZ() <<
    //              ")"
    //              << std::endl;
    //    std::cout << std::fixed << std::setprecision( 3 ) << std::setw( 6 ) << std::showpos        //
    //              << "BiasAlpha: " << est->getBiasAlpha() << ", getGainAcc: " << est->getGainAcc() //
    //              << ", getGainMag: " << est->getGainMag() << ", getSteadyState: " << est->getSteadyState() << std::endl;
    Q_EMIT orientationChanged( Eigen::Quaterniond( q[0], q[1], q[2], q[3] ), CalculationOption::None );

    double processingTime = double( timer.nsecsElapsed() ) / 1.e6;

    Q_EMIT processingTimeChanged( processingTime, CalculationOption::Option::None );
    Q_EMIT maxProcessingTimeChanged( dT * 1000., CalculationOption::Option::None );
  }
}

void
ComplementaryFilterImuFusion::setOrientationCorrection( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
  orientationCorrection = value;
}

void
ComplementaryFilterImuFusion::emitOrientationFromCCF() {
  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

void
ComplementaryFilterImuFusion::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

std::unique_ptr< BlockBase >
ComplementaryFilterImuFusionFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ComplementaryFilterImuFusion >( idHint );

  obj->addInputPort( QStringLiteral( "IMU Data" ), obj.get(), QLatin1StringView( SLOT( setImuData( IMU_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Orientation Correction" ), obj.get(), QLatin1StringView( SLOT( setOrientationCorrection( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Processing Time [ms]" ), obj.get(), QLatin1StringView( SIGNAL( processingTimeChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Max Processing Time [ms]" ), obj.get(), QLatin1StringView( SIGNAL( maxProcessingTimeChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
