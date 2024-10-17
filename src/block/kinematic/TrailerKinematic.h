// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "FixedKinematicPrimitive.h"
#include "TrailerKinematicPrimitive.h"

#include "helpers/eigenHelper.h"

class TrailerKinematic : public BlockBase {
  Q_OBJECT

public:
  explicit TrailerKinematic( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

public Q_SLOTS:
  void setOffsetHookToPivot( VECTOR_SIGNATURE_SLOT );
  void setOffsetPivotToTow( VECTOR_SIGNATURE_SLOT );
  void setMaxJackknifeAngle( NUMBER_SIGNATURE_SLOT );
  void setMaxAngle( NUMBER_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );
  void setPoseInitialMpcPivot( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseHookPointChanged( POSE_SIGNATURE_SIGNAL );
  void posePivotPointChanged( POSE_SIGNATURE_SIGNAL );
  void poseTowPointChanged( POSE_SIGNATURE_SIGNAL );

private:
  TrailerKinematicPrimitive hookToPivot = TrailerKinematicPrimitive( 0, false, "TrailerKinematicPrimitive", BlockBase::TypeColor::System );
  FixedKinematicPrimitive   pivotToTow  = FixedKinematicPrimitive( 0, false, "FixedKinematicPrimitive", BlockBase::TypeColor::System );
};

class TrailerKinematicFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Trailer Kinematic" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
