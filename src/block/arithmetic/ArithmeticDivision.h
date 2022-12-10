// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticDivision : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticDivision() : BlockBase() {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValueA( NUMBER_SIGNATURE_SLOT );
  void setValueB( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double numberA = 1;
  double numberB = 1;
  double result  = 0;
};

class ArithmeticDivisionFactory : public BlockFactory {
  Q_OBJECT

public:
  ArithmeticDivisionFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ArithmeticDivision" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Arithmetic Division" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Arithmetic" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
