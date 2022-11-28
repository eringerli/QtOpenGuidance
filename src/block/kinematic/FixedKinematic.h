// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "FixedKinematicPrimitive.h"
#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class FixedKinematic : public BlockBase {
  Q_OBJECT

public:
  explicit FixedKinematic() : BlockBase() {}

public Q_SLOTS:
  void setOffsetHookToPivot( const Eigen::Vector3d& offset );
  void setOffsetPivotToTow( const Eigen::Vector3d& offset );

  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseHookPointChanged( POSE_SIGNATURE_SIGNAL );
  void posePivotPointChanged( POSE_SIGNATURE_SIGNAL );
  void poseTowPointChanged( POSE_SIGNATURE_SIGNAL );

private:
  FixedKinematicPrimitive hookToPivot;
  FixedKinematicPrimitive pivotToTow;
};

class FixedKinematicFactory : public BlockFactory {
  Q_OBJECT

public:
  FixedKinematicFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Fixed Kinematic" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
