// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class FixedKinematicPrimitive : public BlockBase {
  Q_OBJECT

public:
  explicit FixedKinematicPrimitive() : BlockBase() {}

public Q_SLOTS:
  void setOffset( const Eigen::Vector3d& offset );

  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseChanged( POSE_SIGNATURE_SIGNAL );

public:
  static Eigen::Vector3d calculate( const Eigen::Vector3d& position, const Eigen::Vector3d& offset, const Eigen::Quaterniond& rotation );

public:
  Eigen::Vector3d    positionCalculated    = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientationCalculated = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Vector3d    offset                = Eigen::Vector3d( -1, 0, 0 );
};

class FixedKinematicPrimitiveFactory : public BlockFactory {
  Q_OBJECT

public:
  FixedKinematicPrimitiveFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Fixed Kinematic Primitive" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
