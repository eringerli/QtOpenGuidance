// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AckermannSteering : public BlockBase {
  Q_OBJECT

public:
  explicit AckermannSteering( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {};

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
  AckermannSteeringFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Ackermann Steering" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
};
