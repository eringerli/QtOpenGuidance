// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterOrientation.h"

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/Quaternion.h"

void
SplitterOrientation::emitConfigSignals() {
  BlockBase::emitConfigSignals();

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

std::unique_ptr< BlockBase >
SplitterOrientationFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< SplitterOrientation >( idHint );

  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "X" ), obj.get(), QLatin1StringView( SIGNAL( orientationChangedX( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Y" ), obj.get(), QLatin1StringView( SIGNAL( orientationChangedY( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Z" ), obj.get(), QLatin1StringView( SIGNAL( orientationChangedZ( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "W" ), obj.get(), QLatin1StringView( SIGNAL( orientationChangedW( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
