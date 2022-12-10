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
  explicit TrailerKinematic() : BlockBase() {}

public Q_SLOTS:
  void setOffsetHookToPivot( const Eigen::Vector3d& offset );
  void setOffsetPivotToTow( const Eigen::Vector3d& offset );
  void setMaxJackknifeAngle( NUMBER_SIGNATURE_SLOT );
  void setMaxAngle( NUMBER_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );
  void setPoseInitialMpcPivot( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseHookPointChanged( POSE_SIGNATURE_SIGNAL );
  void posePivotPointChanged( POSE_SIGNATURE_SIGNAL );
  void poseTowPointChanged( POSE_SIGNATURE_SIGNAL );

private:
  TrailerKinematicPrimitive hookToPivot;
  FixedKinematicPrimitive   pivotToTow;
};

class TrailerKinematicFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Trailer Kinematic" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
