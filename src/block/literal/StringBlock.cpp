// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "StringBlock.h"

#include "gui/model/StringBlockModel.h"

#include <QJsonObject>

void
StringBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT stringChanged( string );
}

void
StringBlock::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "String" )] = string;
}

void
StringBlock::fromJSON( const QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "String" )].isString() ) {
    string = valuesObject[QStringLiteral( "String" )].toString();
  }
}

std::unique_ptr< BlockBase >
StringBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< StringBlock >( idHint );

  obj->addOutputPort( QStringLiteral( "String" ), obj.get(), QLatin1StringView( SIGNAL( stringChanged( const QString& ) ) ) );

  model->resetModel();

  return obj;
}
