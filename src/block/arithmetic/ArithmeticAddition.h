// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticAddition : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticAddition() : BlockBase() {}

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

class ArithmeticAdditionFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticAdditionFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ArithmeticAddition" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Arithmetic Addition" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Arithmetic" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
