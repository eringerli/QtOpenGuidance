// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class PoseSynchroniser : public BlockBase {
  Q_OBJECT

public:
  explicit PoseSynchroniser( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

public Q_SLOTS:
  void setPosition( VECTOR_SIGNATURE_SLOT );

  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

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
  PoseSynchroniserFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Pose Synchroniser" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Guidance" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
