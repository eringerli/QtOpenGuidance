// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticAddition.h"

void
ArithmeticAddition::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticAddition::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ArithmeticAddition::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ArithmeticAddition::operation() {
  result = numberA + numberB;
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticAdditionFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticAddition >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "+" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
