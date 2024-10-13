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
  explicit SplitterPose( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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
  SplitterPoseFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "SplitterPose" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Splitter for Pose" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Splitter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
