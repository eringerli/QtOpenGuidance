// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterImu.h"

void
SplitterImu::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  Q_EMIT dtChanged( 0, CalculationOption::Option::None );
  Q_EMIT accelerometerChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
  Q_EMIT gyroChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
  Q_EMIT magnetormeterChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
}

void
SplitterImu::setImu( const double dT, const Eigen::Vector3d& acc, const Eigen::Vector3d& gyr, const Eigen::Vector3d& mag ) {
  Q_EMIT dtChanged( dT, CalculationOption::Option::None );
  Q_EMIT accelerometerChanged( acc, CalculationOption::Option::None );
  Q_EMIT gyroChanged( gyr, CalculationOption::Option::None );
  Q_EMIT magnetormeterChanged( mag, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
SplitterImuFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< SplitterImu >( idHint );

  obj->addInputPort( QStringLiteral( "Imu" ), obj.get(), QLatin1StringView( SLOT( setImu( IMU_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "dT" ), obj.get(), QLatin1StringView( SIGNAL( dtChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Accelerometer" ), obj.get(), QLatin1StringView( SIGNAL( accelerometerChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Gyroscope" ), obj.get(), QLatin1StringView( SIGNAL( gyroChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Magnetometer" ), obj.get(), QLatin1StringView( SIGNAL( magnetormeterChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
