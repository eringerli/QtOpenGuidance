// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticNegation.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ArithmeticNegation::emitConfigSignals() {
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

QNEBlock*
ArithmeticNegationFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticNegation();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Value" ), QLatin1String( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
