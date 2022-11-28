// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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

QJsonObject
VectorObject::toJSON() {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "X" )] = vector.x();
  valuesObject[QStringLiteral( "Y" )] = vector.y();
  valuesObject[QStringLiteral( "Z" )] = vector.z();
  return valuesObject;
}

void
VectorObject::fromJSON( QJsonObject& valuesObject ) {
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
