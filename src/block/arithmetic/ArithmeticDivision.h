// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

class ArithmeticDivision : public BlockBase {
  Q_OBJECT

public:
  explicit ArithmeticDivision( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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
  ArithmeticDivisionFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ArithmeticDivision" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Arithmetic Division" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Arithmetic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
