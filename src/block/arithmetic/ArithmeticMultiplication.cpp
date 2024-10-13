// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticMultiplication.h"

void
ArithmeticMultiplication::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticMultiplication::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ArithmeticMultiplication::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ArithmeticMultiplication::operation() {
  result = numberA * numberB;
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticMultiplicationFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticMultiplication >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "*" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
