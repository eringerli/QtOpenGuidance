// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LogicDivider.h"

void
LogicDivider::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( 0, CalculationOption::Option::None );
}

void
LogicDivider::setSwitch( const bool value ) {
  switchValue = value;
}

void
LogicDivider::setValueA( double number, const CalculationOption::Options ) {
  if( switchValue == false ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

void
LogicDivider::setValueB( double number, const CalculationOption::Options ) {
  if( switchValue == true ) {
    Q_EMIT numberChanged( number, CalculationOption::Option::None );
  }
}

std::unique_ptr< BlockBase >
LogicDividerFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< LogicDivider >( idHint );

  obj->addInputPort( QStringLiteral( "Switch" ), obj.get(), QLatin1StringView( SLOT( setSwitch( ACTION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "A" ), obj.get(), QLatin1StringView( SLOT( setValueA( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "B" ), obj.get(), QLatin1StringView( SLOT( setValueB( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Result" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
