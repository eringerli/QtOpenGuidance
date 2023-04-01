// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticNegation : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticNegation() : BlockBase() {}

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
  ArithmeticNegationFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ArithmeticNegation" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Arithmetic Negation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Arithmetic" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
