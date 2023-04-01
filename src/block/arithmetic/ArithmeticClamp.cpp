// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticClamp.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ArithmeticClamp::emitConfigSignals() {
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

QNEBlock*
ArithmeticClampFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticClamp();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Value" ), QLatin1String( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Minimum" ), QLatin1String( SLOT( setValueMin( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Maximum" ), QLatin1String( SLOT( setValueMax( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
