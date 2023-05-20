// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class SplitterVector : public BlockBase {
  Q_OBJECT

public:
  explicit SplitterVector() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setVector( VECTOR_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChangedX( NUMBER_SIGNATURE_SIGNAL );
  void numberChangedY( NUMBER_SIGNATURE_SIGNAL );
  void numberChangedZ( NUMBER_SIGNATURE_SIGNAL );
};

class SplitterVectorFactory : public BlockFactory {
  Q_OBJECT

public:
  SplitterVectorFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SplitterVector" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Splitter for Vector" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Splitter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
