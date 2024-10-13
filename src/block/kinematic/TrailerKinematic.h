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
  explicit TrailerKinematic( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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
  TrailerKinematicPrimitive hookToPivot = TrailerKinematicPrimitive( 0, false, "TrailerKinematicPrimitive" );
  FixedKinematicPrimitive   pivotToTow  = FixedKinematicPrimitive( 0, false, "FixedKinematicPrimitive" );
};

class TrailerKinematicFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Trailer Kinematic" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
