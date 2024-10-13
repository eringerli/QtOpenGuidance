// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonGreaterThan.h"

void
ComparisonGreaterThan::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stateChanged( result );
}

void
ComparisonGreaterThan::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonGreaterThan::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonGreaterThan::operation() {
  result = numberA > numberB;
  Q_EMIT stateChanged( result );
}

std::unique_ptr< BlockBase >
ComparisonGreaterThanFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ComparisonGreaterThan >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( ">" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  return obj;
}
