// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class SplitterPose : public BlockBase {
  Q_OBJECT

public:
  explicit SplitterPose() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void positionChanged( VECTOR_SIGNATURE_SIGNAL );
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );
};

class SplitterPoseFactory : public BlockFactory {
  Q_OBJECT

public:
  SplitterPoseFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SplitterPose" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Splitter for Pose" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Splitter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
