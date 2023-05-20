// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterImu.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
SplitterImu::emitConfigSignals() {
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

QNEBlock*
SplitterImuFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SplitterImu();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Imu" ), QLatin1String( SLOT( setImu( IMU_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "dT" ), QLatin1String( SIGNAL( dtChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Accelerometer" ), QLatin1String( SIGNAL( accelerometerChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Gyroscope" ), QLatin1String( SIGNAL( gyroChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Magnetometer" ), QLatin1String( SIGNAL( magnetormeterChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );

  b->setBrush( converterColor );

  return b;
}
