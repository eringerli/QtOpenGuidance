// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterOrientation.h"

#include <QBrush>

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/Quaternion.h"
#include "qneblock.h"
#include "qneport.h"

void
SplitterOrientation::emitConfigSignals() {
  Q_EMIT orientationChangedX( 0, CalculationOption::Option::None );
  Q_EMIT orientationChangedY( 0, CalculationOption::Option::None );
  Q_EMIT orientationChangedZ( 0, CalculationOption::Option::None );
  Q_EMIT orientationChangedW( 0, CalculationOption::Option::None );
}

void
SplitterOrientation::setOrientation( const Eigen::Quaterniond& orientation, const CalculationOption::Options ) {
  Q_EMIT orientationChangedX( orientation.x(), CalculationOption::Option::None );
  Q_EMIT orientationChangedY( orientation.y(), CalculationOption::Option::None );
  Q_EMIT orientationChangedZ( orientation.z(), CalculationOption::Option::None );
  Q_EMIT orientationChangedW( orientation.w(), CalculationOption::Option::None );
}

QNEBlock*
SplitterOrientationFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SplitterOrientation();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "X" ), QLatin1String( SIGNAL( orientationChangedX( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Y" ), QLatin1String( SIGNAL( orientationChangedY( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Z" ), QLatin1String( SIGNAL( orientationChangedZ( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "W" ), QLatin1String( SIGNAL( orientationChangedW( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  b->setBrush( converterColor );

  return b;
}
