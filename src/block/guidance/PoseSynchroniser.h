// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class PoseSynchroniser : public BlockBase {
  Q_OBJECT

public:
  explicit PoseSynchroniser() : BlockBase() {}

public Q_SLOTS:
  void setPosition( const Eigen::Vector3d& position );

  void setOrientation( const Eigen::Quaterniond& value );

Q_SIGNALS:
  void poseChanged( POSE_SIGNATURE_SIGNAL );

public:
  virtual void emitConfigSignals() override;

public:
  Eigen::Vector3d    position    = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientation = Eigen::Quaterniond( 0, 0, 0, 0 );
};

class PoseSynchroniserFactory : public BlockFactory {
  Q_OBJECT

public:
  PoseSynchroniserFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Pose Synchroniser" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
