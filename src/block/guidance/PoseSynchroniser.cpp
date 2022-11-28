// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PoseSynchroniser.h"

#include "qneblock.h"
#include "qneport.h"

void
PoseSynchroniser::setPosition( const Eigen::Vector3d& position ) {
  this->position = position;
  //      QElapsedTimer timer;
  //      timer.start();
  Q_EMIT poseChanged( this->position, orientation, CalculationOption::None );
  //      qDebug() << "Cycle Time PoseSynchroniser:  " << timer.nsecsElapsed() << "ns";
}

void
PoseSynchroniser::setOrientation( const Eigen::Quaterniond& value ) {
  orientation = value;
}

void
PoseSynchroniser::emitConfigSignals() {
  Q_EMIT poseChanged( position, orientation, CalculationOption::None );
}

QNEBlock*
PoseSynchroniserFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new PoseSynchroniser();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
