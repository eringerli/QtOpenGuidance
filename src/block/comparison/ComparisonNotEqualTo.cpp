// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ComparisonNotEqualTo.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ComparisonNotEqualTo::emitConfigSignals() {
  Q_EMIT stateChanged( result );
}

void
ComparisonNotEqualTo::setValueA( double number, const CalculationOption::Options ) {
  numberA = number;
  operation();
}

void
ComparisonNotEqualTo::setValueB( double number, const CalculationOption::Options ) {
  numberB = number;
  operation();
}

void
ComparisonNotEqualTo::operation() {
  result = !qFuzzyCompare( numberA, numberB );
  Q_EMIT stateChanged( result );
}

QNEBlock*
ComparisonNotEqualToFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ComparisonNotEqualTo();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  b->addPort( QStringLiteral( "!=" ), QLatin1String(), false, QNEPort::NoBullet );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( stateChanged( ACTION_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
