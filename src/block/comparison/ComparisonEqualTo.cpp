// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonEqualTo.h"

void
ComparisonEqualTo::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stateChanged( result );
}

void
ComparisonEqualTo::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonEqualTo::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonEqualTo::operation() {
  result = qFuzzyCompare( numberA, numberB );
  Q_EMIT stateChanged( result );
}

std::unique_ptr< BlockBase >
ComparisonEqualToFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ComparisonEqualTo >( idHint );
  // obj->addPort\( QStringLiteral\( "(.*)" \), QLatin1StringView\(\), false, QNEPort::NoBullet \);
  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "==" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  return obj;
}
