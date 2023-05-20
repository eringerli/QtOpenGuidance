// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SchmittTriggerBlock.h"

#include <QBrush>

#include "qneblock.h"
#include "qneport.h"

void
SchmittTriggerBlock::emitConfigSignals() {
  Q_EMIT numberChanged( 0, CalculationOption::Option::None );
}

void
SchmittTriggerBlock::setValue( const double number, const CalculationOption::Options ) {
  auto result = schmittTrigger.trigger( number );

  Q_EMIT numberChanged( result, CalculationOption::Option::None );
}

void
SchmittTriggerBlock::setThreshold( const double number, const CalculationOption::Options ) {
  schmittTrigger.setThreshold( number );
}

void
SchmittTriggerBlock::setValueHysteresis( const double number, const CalculationOption::Options ) {
  schmittTrigger.setHysteresis( number );
}

void
SchmittTriggerBlock::setValueLowerThreshold( const double number, const CalculationOption::Options ) {
  schmittTrigger.setLowerThreshold( number );
}

void
SchmittTriggerBlock::setValueUpperThreshold( const double number, const CalculationOption::Options ) {
  schmittTrigger.setUpperThreshold( number );
}

QNEBlock*
SchmittTriggerBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SchmittTriggerBlock();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Value" ), QLatin1String( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Threshold" ), QLatin1String( SLOT( setThreshold( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Result" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  b->addInputPort( QStringLiteral( "Hysteresis" ), QLatin1String( SLOT( setValueHysteresis( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Lower Threshold" ), QLatin1String( SLOT( setLowerThreshold( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Upper Threshold" ), QLatin1String( SLOT( setUpperThreshold( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( arithmeticColor );

  return b;
}
