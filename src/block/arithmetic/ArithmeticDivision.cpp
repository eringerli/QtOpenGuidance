// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticDivision.h"

void
ArithmeticDivision::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticDivision::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ArithmeticDivision::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ArithmeticDivision::operation() {
  if( !qIsNull( numberB ) ) {
    result = numberA / numberB;
  }

  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticDivisionFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticDivision >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "/" ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
