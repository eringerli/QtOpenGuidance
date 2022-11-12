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

#include "block/BlockBase.h"
#include "helpers/anglesHelper.h"

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AngularVelocityLimiter : public BlockBase {
  Q_OBJECT

public:
  explicit AngularVelocityLimiter() = default;

public Q_SLOTS:
  void setMaxAngularVelocity( NUMBER_SIGNATURE_SLOT );
  void setMaxSteeringAngle( NUMBER_SIGNATURE_SLOT );
  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setVelocity( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void maxSteeringAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void minRadiusChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  double maxAngularVelocity  = degreesToRadians( 5. );
  double maxSteeringAngleRad = degreesToRadians( 40. );
  double wheelbase           = 2.4f;
};

class AngularVelocityLimiterFactory : public BlockFactory {
  Q_OBJECT

public:
  AngularVelocityLimiterFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Angular Velocity Limiter" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
