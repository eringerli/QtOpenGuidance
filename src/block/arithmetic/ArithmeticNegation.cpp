// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticNegation.h"

void
ArithmeticNegation::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticNegation::setValue( double number, const CalculationOption::Options ) {
  this->number = number;
  operation();
}

void
ArithmeticNegation::operation() {
  result = number * -1.;
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticNegationFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticNegation >( idHint );

  obj->addInputPort( QStringLiteral( "Value" ), obj.get(), QLatin1StringView( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
