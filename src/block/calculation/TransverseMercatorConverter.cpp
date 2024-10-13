// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TransverseMercatorConverter.h"

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
  BlockBase::emitConfigSignals();

  Q_EMIT positionChanged( Eigen::Vector3d( 0, 0, 0 ), CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
TransverseMercatorConverterFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< TransverseMercatorConverter >( idHint, tmw );

  obj->addInputPort( QStringLiteral( "WGS84 Position" ), obj.get(), QLatin1StringView( SLOT( setWGS84Position( VECTOR_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Position" ), obj.get(), QLatin1StringView( SIGNAL( positionChanged( VECTOR_SIGNATURE ) ) ) );

  return obj;
}
