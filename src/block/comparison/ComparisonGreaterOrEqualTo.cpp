// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonGreaterOrEqualTo.h"

void
ComparisonGreaterOrEqualTo::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stateChanged( result );
}

void
ComparisonGreaterOrEqualTo::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonGreaterOrEqualTo::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonGreaterOrEqualTo::operation() {
  result = numberA >= numberB;
  Q_EMIT stateChanged( result );
}

std::unique_ptr< BlockBase >
ComparisonGreaterOrEqualToFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ComparisonGreaterOrEqualTo >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( ">=" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  return obj;
}
