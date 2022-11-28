// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AngularVelocityLimiter.h"

#include "qneblock.h"
#include "qneport.h"

void
AngularVelocityLimiter::setMaxAngularVelocity( double maxAngularVelocity, const CalculationOption::Options ) {
  this->maxAngularVelocity = degreesToRadians( maxAngularVelocity );
}

void
AngularVelocityLimiter::setMaxSteeringAngle( double maxSteeringAngle, const CalculationOption::Options ) {
  this->maxSteeringAngleRad = degreesToRadians( maxSteeringAngle );
}

void
AngularVelocityLimiter::setWheelbase( double wheelbase, const CalculationOption::Options ) {
  this->wheelbase = wheelbase;
}

void
AngularVelocityLimiter::setVelocity( double velocity, const CalculationOption::Options ) {
  double maxSteeringAngleFromAngularRad = std::abs( std::atan( maxAngularVelocity * wheelbase / velocity ) );
  double maxSteeringAngleMinRad         = std::min( maxSteeringAngleFromAngularRad, maxSteeringAngleRad );
  Q_EMIT maxSteeringAngleChanged( radiansToDegrees( maxSteeringAngleMinRad ), CalculationOption::Option::None );

  double minRadiusPivotPoint = wheelbase / std::tan( maxSteeringAngleMinRad );
  // the minimal radius is from the turning point to the middle of steering axle ->
  // pythagoras
  Q_EMIT minRadiusChanged( std::sqrt( ( minRadiusPivotPoint * minRadiusPivotPoint ) + ( wheelbase * wheelbase ) ),
                           CalculationOption::Option::None );
}

QNEBlock*
AngularVelocityLimiterFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new AngularVelocityLimiter();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Max Angular Velocity" ), QLatin1String( SLOT( setMaxAngularVelocity( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SIGNAL( maxSteeringAngleChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Min Radius" ), QLatin1String( SIGNAL( minRadiusChanged( NUMBER_SIGNATURE ) ) ) );

  return b;
}
