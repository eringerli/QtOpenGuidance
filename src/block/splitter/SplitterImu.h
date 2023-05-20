// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class SplitterImu : public BlockBase {
  Q_OBJECT

public:
  explicit SplitterImu() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setImu( IMU_SIGNATURE_SLOT );

Q_SIGNALS:
  void dtChanged( NUMBER_SIGNATURE_SIGNAL );
  void accelerometerChanged( VECTOR_SIGNATURE_SIGNAL );
  void gyroChanged( VECTOR_SIGNATURE_SIGNAL );
  void magnetormeterChanged( VECTOR_SIGNATURE_SIGNAL );
};

class SplitterImuFactory : public BlockFactory {
  Q_OBJECT

public:
  SplitterImuFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SplitterImu" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Splitter for Imu" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Splitter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
