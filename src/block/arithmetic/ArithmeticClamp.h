// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"
#include <limits>

class ArithmeticClamp : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticClamp( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValue( NUMBER_SIGNATURE_SLOT );
  void setValueMin( NUMBER_SIGNATURE_SLOT );
  void setValueMax( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double number    = 0;
  double numberMin = -std::numeric_limits< double >::infinity();
  double numberMax = std::numeric_limits< double >::infinity();
  double result    = 0;
};

class ArithmeticClampFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticClampFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ArithmeticClamp" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Arithmetic Clamp" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Arithmetic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
