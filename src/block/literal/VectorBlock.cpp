// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "VectorBlock.h"

#include "kinematic/CalculationOptions.h"

#include "gui/model/OrientationBlockModel.h"

#include <QJsonObject>

void
VectorBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT vectorChanged( vector, CalculationOption::Option::None );
}

void
VectorBlock::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "X" )] = vector.x();
  valuesObject[QStringLiteral( "Y" )] = vector.y();
  valuesObject[QStringLiteral( "Z" )] = vector.z();
}

void
VectorBlock::fromJSON( const QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "X" )].isDouble() ) {
    vector.x() = valuesObject[QStringLiteral( "X" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Y" )].isDouble() ) {
    vector.y() = valuesObject[QStringLiteral( "Y" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Z" )].isDouble() ) {
    vector.z() = valuesObject[QStringLiteral( "Z" )].toDouble();
  }
}

std::unique_ptr< BlockBase >
VectorBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< VectorBlock >( idHint );

  obj->addOutputPort( QStringLiteral( "Position" ), obj.get(), QLatin1StringView( SIGNAL( vectorChanged( VECTOR_SIGNATURE ) ) ) );

  model->resetModel();

  return obj;
}
