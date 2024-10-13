// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticAbsolute.h"

void
ArithmeticAbsolute::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
ArithmeticAbsolute::setValueA( double number, const CalculationOption::Options ) {
  this->number = number;
  operation();
}

void
ArithmeticAbsolute::operation() {
  result = std::abs( number );
  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
ArithmeticAbsoluteFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ArithmeticAbsolute >( idHint );

  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
