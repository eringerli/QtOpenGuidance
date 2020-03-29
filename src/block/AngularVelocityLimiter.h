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

#pragma once

#include <QObject>

#include "BlockBase.h"

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AngularVelocityLimiter : public BlockBase {
    Q_OBJECT

  public:
    explicit AngularVelocityLimiter() = default;

  public slots:
    void setMaxAngularVelocity( double maxAngularVelocity ) {
      this->maxAngularVelocity = qDegreesToRadians( maxAngularVelocity );
    }

    void setMaxSteeringAngle( double maxSteeringAngle ) {
      this->maxSteeringAngleRad = qDegreesToRadians( maxSteeringAngle );
    }

    void setWheelbase( double wheelbase ) {
      this->wheelbase = wheelbase;
    }

    void setVelocity( double velocity ) {
      double maxSteeringAngleFromAngularRad = std::abs( std::atan( maxAngularVelocity * wheelbase / velocity ) );
      double maxSteeringAngleMinRad = std::min( maxSteeringAngleFromAngularRad, maxSteeringAngleRad );
      emit maxSteeringAngleChanged( qRadiansToDegrees( maxSteeringAngleMinRad ) );

      double minRadiusPivotPoint = wheelbase / std::tan( maxSteeringAngleMinRad );
      // the minimal radius is from the turning point to the middle of steering axle -> pythagoras
      emit minRadiusChanged( std::sqrt( ( minRadiusPivotPoint * minRadiusPivotPoint ) + ( wheelbase * wheelbase ) ) );
    }

  signals:
    void maxSteeringAngleChanged( double );
    void minRadiusChanged( double );

  private:
    double maxAngularVelocity = qDegreesToRadians( double( 5 ) );
    double maxSteeringAngleRad = qDegreesToRadians( double( 40 ) );
    double wheelbase = 2.4f;
};

class AngularVelocityLimiterFactory : public BlockFactory {
    Q_OBJECT

  public:
    AngularVelocityLimiterFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Angular Velocity Limiter" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
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

  private:
};
