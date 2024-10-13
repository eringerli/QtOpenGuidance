// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticSubtraction.h"

void
ArithmeticSubtraction::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticSubtraction::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ArithmeticSubtraction::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ArithmeticSubtraction::operation() {
  result = numberA - numberB;
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticSubtractionFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticSubtraction >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "-" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
