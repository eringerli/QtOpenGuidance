// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticClamp.h"

void
ArithmeticClamp::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticClamp::setValue( double number, const CalculationOption::Options ) {
  this->number = number;
  operation();
}

void
ArithmeticClamp::setValueMin( double number, const CalculationOption::Options ) {
  numberMin = number;
  operation();
}

void
ArithmeticClamp::setValueMax( double number, const CalculationOption::Options ) {
  numberMax = number;
  operation();
}

void
ArithmeticClamp::operation() {
  result = std::clamp( number, numberMin, numberMax );
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticClampFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< ArithmeticClamp >( idHint );

  obj->addInputPort( QStringLiteral( "Value" ), obj.get(), QLatin1StringView( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Minimum" ), obj.get(), QLatin1StringView( SLOT( setValueMin( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Maximum" ), obj.get(), QLatin1StringView( SLOT( setValueMax( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
