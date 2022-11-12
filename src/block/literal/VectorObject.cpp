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

#include "VectorObject.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/model/OrientationBlockModel.h"

#include <QBrush>
#include <QJsonObject>

void
VectorObject::emitConfigSignals() {
  Q_EMIT vectorChanged( vector );
}

void
VectorObject::toJSON( QJsonObject& json ) {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "X" )] = vector.x();
  valuesObject[QStringLiteral( "Y" )] = vector.y();
  valuesObject[QStringLiteral( "Z" )] = vector.z();
  json[QStringLiteral( "values" )]    = valuesObject;
}

void
VectorObject::fromJSON( QJsonObject& json ) {
  if( json[QStringLiteral( "values" )].isObject() ) {
    QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

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
}

QNEBlock*
VectorFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new VectorObject();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( vectorChanged( const Eigen::Vector3d& ) ) ) );

  b->setBrush( valueColor );

  model->resetModel();

  return b;
}
