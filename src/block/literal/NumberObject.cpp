// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NumberObject.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/model/NumberBlockModel.h"

#include <QBrush>
#include <QJsonObject>

void
NumberObject::emitConfigSignals() {
  Q_EMIT numberChanged( number, CalculationOption::Option::None );
}

QJsonObject
NumberObject::toJSON() const {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "Number" )] = number;
  return valuesObject;
}

void
NumberObject::fromJSON( QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "Number" )].isDouble() ) {
    number = valuesObject[QStringLiteral( "Number" )].toDouble();
  }
}

QNEBlock*
NumberFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new NumberObject();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( numberChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( valueColor );

  model->resetModel();

  return b;
}
