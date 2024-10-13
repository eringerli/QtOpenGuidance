// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SchmittTriggerBlock.h"

void
SchmittTriggerBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
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

std::unique_ptr< BlockBase >
SchmittTriggerBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< SchmittTriggerBlock >( idHint );

  obj->addInputPort( QStringLiteral( "Value" ), obj.get(), QLatin1StringView( SLOT( setValue( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Threshold" ), obj.get(), QLatin1StringView( SLOT( setThreshold( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  obj->addInputPort( QStringLiteral( "Hysteresis" ), obj.get(), QLatin1StringView( SLOT( setValueHysteresis( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Lower Threshold" ), obj.get(), QLatin1StringView( SLOT( setLowerThreshold( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Upper Threshold" ), obj.get(), QLatin1StringView( SLOT( setUpperThreshold( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
