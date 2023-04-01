// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticAbsolute.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ArithmeticAbsolute::emitConfigSignals() {
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

QNEBlock*
ArithmeticAbsoluteFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticAbsolute();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
