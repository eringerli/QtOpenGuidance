// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class SplitterOrientation : public BlockBase {
  Q_OBJECT

public:
  explicit SplitterOrientation() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

Q_SIGNALS:
  void orientationChangedX( NUMBER_SIGNATURE_SIGNAL );
  void orientationChangedY( NUMBER_SIGNATURE_SIGNAL );
  void orientationChangedZ( NUMBER_SIGNATURE_SIGNAL );
  void orientationChangedW( NUMBER_SIGNATURE_SIGNAL );
};

class SplitterOrientationFactory : public BlockFactory {
  Q_OBJECT

public:
  SplitterOrientationFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SplitterOrientation" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Splitter for Orientation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Splitter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
