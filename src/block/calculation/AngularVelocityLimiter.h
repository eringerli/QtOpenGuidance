// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"
#include "helpers/anglesHelper.h"

// all the formulas are from https://www.xarg.org/book/kinematics/ackerman-steering/

class AngularVelocityLimiter : public BlockBase {
  Q_OBJECT

public:
  explicit AngularVelocityLimiter( const BlockBaseId          idHint,
                                   const bool                 systemBlock,
                                   const QString              type,
                                   const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {};

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
  AngularVelocityLimiterFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Angular Velocity Limiter" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
