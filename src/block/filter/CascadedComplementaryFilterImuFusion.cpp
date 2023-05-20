// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CascadedComplementaryFilterImuFusion.h"

#include "kinematic/CalculationOptions.h"
#include "qneblock.h"
#include "qneport.h"

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

CascadedComplementaryFilterImuFusion::CascadedComplementaryFilterImuFusion() { elapsedTimer.start(); }

void
CascadedComplementaryFilterImuFusion::setImuData( const double           dT,
                                                  const Eigen::Vector3d& accelerometerData,
                                                  const Eigen::Vector3d& gyroData,
                                                  const Eigen::Vector3d& magnetometerData ) {
  // TODO: Ausklammern
  Eigen::Vector3d accMagOrientation = Eigen::Vector3d( 0, 0, 0 );
  getRoll( accMagOrientation )      = std::atan2( accelerometerData.y(), accelerometerData.z() );
  getPitch( accMagOrientation )     = std::atan2( -accelerometerData.x(),
                                              accelerometerData.y() * std::sin( getRoll( orientationHat ) ) +
                                                accelerometerData.z() * std::cos( getRoll( orientationHat ) ) );
  getYaw( accMagOrientation ) =
    std::atan2( magnetometerData.z() * std::sin( getRoll( orientationHat ) ) - //
                  magnetometerData.y() * std::cos( getRoll( orientationHat ) ),
                magnetometerData.x() * std::cos( getPitch( orientationHat ) ) +
                  magnetometerData.y() * std::sin( getRoll( orientationHat ) ) * std::sin( getPitch( orientationHat ) ) +
                  magnetometerData.z() * std::cos( getRoll( orientationHat ) ) * std::sin( getPitch( orientationHat ) ) );

  Eigen::Vector3d gyroOrientationDot = Eigen::Vector3d( 0, 0, 0 );
  getRoll( gyroOrientationDot )      = getRoll( gyroData ) + //
                                  getPitch( gyroData ) * std::sin( getRoll( orientationHat ) ) * std::tan( getPitch( orientationHat ) ) +
                                  getYaw( gyroData ) * std::cos( getRoll( orientationHat ) ) * std::tan( getPitch( orientationHat ) );

  getPitch( gyroOrientationDot ) = getPitch( gyroData ) * std::cos( getRoll( orientationHat ) ) + //
                                   getYaw( gyroData ) * std::sin( getPitch( orientationHat ) );

  getYaw( gyroOrientationDot ) = getRoll( gyroData ) + //
                                 getPitch( gyroData ) * std::sin( getRoll( orientationHat ) ) * sec( getPitch( orientationHat ) ) +
                                 getYaw( gyroData ) * std::cos( getRoll( orientationHat ) ) * sec( getPitch( orientationHat ) );

  auto error = orientationHat - accMagOrientation;

  orientationErrorIntegral += error * dT;

  auto deltaOmega = error * kP + orientationErrorIntegral * kI;

  auto gyroOrientationDotCorrected = gyroOrientationDot - deltaOmega;

  orientationGyroIntegral += gyroOrientationDotCorrected * dT;

  orientationHat = accMagOrientation * alphaInverse + orientationGyroIntegral * alpha;

  Q_EMIT orientationChanged( taitBryanToQuaternion( orientationHat ), CalculationOption::Option::None );
}

void
CascadedComplementaryFilterImuFusion::setOrientationCorrection( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
  orientationCorrection = value;
}

void
CascadedComplementaryFilterImuFusion::emitOrientationFromCCF() {
  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

void
CascadedComplementaryFilterImuFusion::emitConfigSignals() {
  //  Q_EMIT orientationChanged( this->position, this->orientation, CalculationOption::None );
}

QNEBlock*
CascadedComplementaryFilterImuFusionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new CascadedComplementaryFilterImuFusion();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "IMU Data" ), QLatin1String( SLOT( setImuData( IMU_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation Correction" ), QLatin1String( SLOT( setOrientationCorrection( ORIENTATION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  return b;
}
