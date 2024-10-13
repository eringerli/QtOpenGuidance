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
  explicit SplitterImu( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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
  SplitterImuFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "SplitterImu" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Splitter for Imu" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Splitter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
