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
  explicit SplitterOrientation( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

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
  SplitterOrientationFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Parser; }

  QString getNameOfFactory() const override { return QStringLiteral( "SplitterOrientation" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Splitter for Orientation" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Splitter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
