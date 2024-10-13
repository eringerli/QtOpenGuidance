// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PoseSynchroniser.h"

void
PoseSynchroniser::setPosition( const Eigen::Vector3d& position, const CalculationOption::Options options ) {
  this->position = position;
  //      QElapsedTimer timer;
  //      timer.start();
  Q_EMIT poseChanged( this->position, orientation, options );
  //      qDebug() << "Cycle Time PoseSynchroniser:  " << timer.nsecsElapsed() << "ns";
}

void
PoseSynchroniser::setOrientation( const Eigen::Quaterniond& value, const CalculationOption::Options ) {
  orientation = value;
}

void
PoseSynchroniser::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT poseChanged( position, orientation, CalculationOption::None );
}

std::unique_ptr< BlockBase >
PoseSynchroniserFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< PoseSynchroniser >( idHint );

  obj->addInputPort( QStringLiteral( "Position" ), obj.get(), QLatin1StringView( SLOT( setPosition( VECTOR_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return obj;
}
