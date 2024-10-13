// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonLessThan.h"

void
ComparisonLessThan::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stateChanged( result );
}

void
ComparisonLessThan::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonLessThan::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonLessThan::operation() {
  result = numberA < numberB;
  Q_EMIT stateChanged( result );
}

std::unique_ptr< BlockBase >
ComparisonLessThanFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ComparisonLessThan >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "<" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  return obj;
}
