// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TransverseMercatorConverter.h"

#include "qneblock.h"
#include "qneport.h"

void
TransverseMercatorConverter::setWGS84Position( const Eigen::Vector3d& position, const CalculationOption::Options ) {
  double x = 0;
  double y = 0;
  double z = 0;
  tmw->Forward( position.x(), position.y(), position.z(), x, y, z );

  Q_EMIT positionChanged( Eigen::Vector3d( x, y, z ), CalculationOption::Option::None );
}

void
TransverseMercatorConverter::emitConfigSignals() {
  Q_EMIT positionChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
}

QNEBlock*
TransverseMercatorConverterFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TransverseMercatorConverter( tmw );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( VECTOR_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( VECTOR_SIGNATURE ) ) ) );

  return b;
}
