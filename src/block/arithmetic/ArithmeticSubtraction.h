// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticSubtraction : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticSubtraction( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValueA( NUMBER_SIGNATURE_SLOT );
  void setValueB( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double numberA = 0;
  double numberB = 0;
  double result  = 0;
};

class ArithmeticSubtractionFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticSubtractionFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ArithmeticSubtraction" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Arithmetic Subtraction" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Arithmetic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
