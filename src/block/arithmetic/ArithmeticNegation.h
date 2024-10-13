// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticNegation : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticNegation( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValue( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double number = 0;
  double result = 0;
};

class ArithmeticNegationFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticNegationFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ArithmeticNegation" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Arithmetic Negation" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Arithmetic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
