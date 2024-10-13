// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValveNumber.h"

void
ValveNumber::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( 0, CalculationOption::Option::None );
}

void
ValveNumber::setSwitch( const bool value ) {
  switchValue = value;
}

void
ValveNumber::setValueA( const double number, const CalculationOption::Options ) {
  if( switchValue == false ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

void
ValveNumber::setValueB( const double number, const CalculationOption::Options ) {
  if( switchValue == true ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

std::unique_ptr< BlockBase >
ValveNumberFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ValveNumber >( idHint );

  obj->addInputPort( QStringLiteral( "Switch" ), obj.get(), QLatin1StringView( SLOT( setSwitch( ACTION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
