// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ArithmeticSubtraction.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ArithmeticSubtraction::emitConfigSignals() {
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

QNEBlock*
ArithmeticSubtractionFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ArithmeticSubtraction();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  b->addPort( QStringLiteral( "-" ), QLatin1String(), false, QNEPort::NoBullet );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
