// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticAbsolute : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticAbsolute() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValueA( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double number = 0;
  double result = 0;
};

class ArithmeticAbsoluteFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticAbsoluteFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ArithmeticAbsolute" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Arithmetic Absolute" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Arithmetic" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
