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
  explicit SplitterVector( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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
  SplitterVectorFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "SplitterVector" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Splitter for Vector" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Splitter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
