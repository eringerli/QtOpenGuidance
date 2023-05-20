// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StringObject.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/model/StringBlockModel.h"

#include <QBrush>
#include <QJsonObject>

void
StringObject::emitConfigSignals() {
  Q_EMIT stringChanged( string );
}

QJsonObject
StringObject::toJSON() const {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "String" )] = string;
  return valuesObject;
}

void
StringObject::fromJSON( QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "String" )].isString() ) {
    string = valuesObject[QStringLiteral( "String" )].toString();
  }
}

QNEBlock*
StringFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new StringObject();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addOutputPort( QStringLiteral( "String" ), QLatin1String( SIGNAL( stringChanged( const QString& ) ) ) );

  b->setBrush( valueColor );

  model->resetModel();

  return b;
}
