// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterPose.h"

#include <QBrush>

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/Quaternion.h"
#include "qneblock.h"
#include "qneport.h"

void
SplitterPose::emitConfigSignals() {
  Q_EMIT positionChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
  Q_EMIT orientationChanged( Eigen::Quaterniond( 0, 0, 0, 0 ), CalculationOption::Option::None );
}

void
SplitterPose::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options ) {
  Q_EMIT positionChanged( position, CalculationOption::Option::None );
  Q_EMIT orientationChanged( orientation, CalculationOption::Option::None );
}

QNEBlock*
SplitterPoseFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SplitterPose();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( VECTOR_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE_SIGNAL ) ) ) );

  b->setBrush( converterColor );

  return b;
}
