// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "StringObject.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/model/StringBlockModel.h"

#include <QBrush>
#include <QJsonObject>

void StringObject::emitConfigSignals() {
  Q_EMIT stringChanged( string );
}

void StringObject::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "String" )] = string;
  json[QStringLiteral( "values" )] = valuesObject;
}

void StringObject::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

    if( valuesObject[QStringLiteral( "String" )].isString() ) {
      string = valuesObject[QStringLiteral( "String" )].toString();
    }
  }
}

QNEBlock* StringFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new StringObject();
  auto* b = createBaseBlock( scene, obj, id );

  b->addOutputPort( QStringLiteral( "String" ), QLatin1String( SIGNAL( stringChanged( const QString& ) ) ) );

  b->setBrush( valueColor );

  model->resetModel();

  return b;
}
