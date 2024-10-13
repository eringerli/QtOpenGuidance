// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NumberBlock.h"

#include "gui/model/NumberBlockModel.h"

#include <QJsonObject>

void
NumberBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT numberChanged( number, CalculationOption::Option::None );
}

void
NumberBlock::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "Number" )] = number;
}

void
NumberBlock::fromJSON( const QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "Number" )].isDouble() ) {
    number = valuesObject[QStringLiteral( "Number" )].toDouble();
  }
}

std::unique_ptr< BlockBase >
NumberBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< NumberBlock >( idHint );

  obj->addOutputPort( QStringLiteral( "Number" ), obj.get(), QLatin1StringView( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  model->resetModel();

  return obj;
}
