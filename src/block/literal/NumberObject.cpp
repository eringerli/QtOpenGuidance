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

void
NumberObject::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "Number" )] = number;
  json[QStringLiteral( "values" )]         = valuesObject;
}

void
NumberObject::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

    if( valuesObject[QStringLiteral( "Number" )].isDouble() ) {
      number = valuesObject[QStringLiteral( "Number" )].toDouble();
    }
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
