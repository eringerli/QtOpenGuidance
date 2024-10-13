// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SplitterVector.h"

void
SplitterVector::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  Q_EMIT numberChangedX( 0, CalculationOption::Option::None );
  Q_EMIT numberChangedY( 0, CalculationOption::Option::None );
  Q_EMIT numberChangedZ( 0, CalculationOption::Option::None );
}

void
SplitterVector::setVector( const Eigen::Vector3d& vector, const CalculationOption::Options ) {
  Q_EMIT numberChangedX( vector.x(), CalculationOption::Option::None );
  Q_EMIT numberChangedY( vector.y(), CalculationOption::Option::None );
  Q_EMIT numberChangedZ( vector.z(), CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
SplitterVectorFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< SplitterVector >( idHint );

  obj->addInputPort( QStringLiteral( "Vector" ), obj.get(), QLatin1StringView( SLOT( setVector( VECTOR_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "X" ), obj.get(), QLatin1StringView( SIGNAL( numberChangedX( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Y" ), obj.get(), QLatin1StringView( SIGNAL( numberChangedY( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Z" ), obj.get(), QLatin1StringView( SIGNAL( numberChangedZ( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
