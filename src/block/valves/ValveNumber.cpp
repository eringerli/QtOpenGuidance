// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValveNumber.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
ValveNumber::emitConfigSignals() {
  Q_EMIT numberChanged( 0, CalculationOption::Option::None );
}

void
ValveNumber::setSwitch( const bool value ) {
  switchValue = value;
}

void
ValveNumber::setValueA( double number, const CalculationOption::Options ) {
  if( switchValue == false ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

void
ValveNumber::setValueB( double number, const CalculationOption::Options ) {
  if( switchValue == true ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

QNEBlock*
ValveNumberFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new ValveNumber();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Switch" ), QLatin1String( SLOT( setSwitch( ACTION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "A" ), QLatin1String( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "B" ), QLatin1String( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
