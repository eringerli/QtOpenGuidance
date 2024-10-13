// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonLessOrEqualTo.h"

void
ComparisonLessOrEqualTo::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stateChanged( result );
}

void
ComparisonLessOrEqualTo::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonLessOrEqualTo::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonLessOrEqualTo::operation() {
  result = numberA <= numberB;
  Q_EMIT stateChanged( result );
}

std::unique_ptr< BlockBase >
ComparisonLessOrEqualToFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ComparisonLessOrEqualTo >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "<=" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  return obj;
}
