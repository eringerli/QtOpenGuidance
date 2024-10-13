// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AngularVelocityLimiter.h"

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

std::unique_ptr< BlockBase >
AngularVelocityLimiterFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< AngularVelocityLimiter >( idHint );

  obj->addInputPort(
    QStringLiteral( "Max Angular Velocity" ), obj.get(), QLatin1StringView( SLOT( setMaxAngularVelocity( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Max Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setMaxSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Length Wheelbase" ), obj.get(), QLatin1StringView( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Velocity" ), obj.get(), QLatin1StringView( SLOT( setVelocity( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Max Steering Angle" ), obj.get(), QLatin1StringView( SIGNAL( maxSteeringAngleChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Min Radius" ), obj.get(), QLatin1StringView( SIGNAL( minRadiusChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
