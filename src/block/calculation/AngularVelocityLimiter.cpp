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

#include "AngularVelocityLimiter.h"

#include "qneblock.h"
#include "qneport.h"

void AngularVelocityLimiter::setMaxAngularVelocity( double maxAngularVelocity ) {
  this->maxAngularVelocity = degreesToRadians( maxAngularVelocity );
}

void AngularVelocityLimiter::setMaxSteeringAngle( double maxSteeringAngle ) {
  this->maxSteeringAngleRad = degreesToRadians( maxSteeringAngle );
}

void AngularVelocityLimiter::setWheelbase( double wheelbase ) {
  this->wheelbase = wheelbase;
}

void AngularVelocityLimiter::setVelocity( double velocity ) {
  double maxSteeringAngleFromAngularRad = std::abs( std::atan( maxAngularVelocity * wheelbase / velocity ) );
  double maxSteeringAngleMinRad = std::min( maxSteeringAngleFromAngularRad, maxSteeringAngleRad );
  Q_EMIT maxSteeringAngleChanged( radiansToDegrees( maxSteeringAngleMinRad ) );

  double minRadiusPivotPoint = wheelbase / std::tan( maxSteeringAngleMinRad );
  // the minimal radius is from the turning point to the middle of steering axle -> pythagoras
  Q_EMIT minRadiusChanged( std::sqrt( ( minRadiusPivotPoint * minRadiusPivotPoint ) + ( wheelbase * wheelbase ) ) );
}

QNEBlock* AngularVelocityLimiterFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new AngularVelocityLimiter();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Max Angular Velocity" ), QLatin1String( SLOT( setMaxAngularVelocity( double ) ) ) );
  b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( double ) ) ) );
  b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( double ) ) ) );
  b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( double ) ) ) );

  b->addOutputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SIGNAL( maxSteeringAngleChanged( double ) ) ) );
  b->addOutputPort( QStringLiteral( "Min Radius" ), QLatin1String( SIGNAL( minRadiusChanged( double ) ) ) );

  return b;
}
