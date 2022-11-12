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

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AckermannSteering : public BlockBase {
  Q_OBJECT

public:
  explicit AckermannSteering() = default;

public Q_SLOTS:
  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setTrackWidth( NUMBER_SIGNATURE_SLOT );

  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );
  void setSteeringAngleLeft( NUMBER_SIGNATURE_SLOT );
  void setSteeringAngleRight( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void steeringAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void steeringAngleChangedLeft( NUMBER_SIGNATURE_SIGNAL );
  void steeringAngleChangedRight( NUMBER_SIGNATURE_SIGNAL );

private:
  double m_wheelbase  = 2.4f;
  double m_trackWidth = 2.0f;

  // formula: m_trackWidth / ( 2.0f * m_wheelbase )
  double m_correction = 2 / ( 2 * 2.4 );
};

class AckermannSteeringFactory : public BlockFactory {
  Q_OBJECT

public:
  AckermannSteeringFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Ackermann Steering" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
};
