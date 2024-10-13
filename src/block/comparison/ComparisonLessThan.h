// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ComparisonLessThan : public BlockBase {
  Q_OBJECT

public:
  explicit ComparisonLessThan( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValueA( NUMBER_SIGNATURE_SLOT );
  void setValueB( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void stateChanged( ACTION_SIGNATURE_SIGNAL );

private:
  void operation();

public:
  double numberA = 0;
  double numberB = 0;
  bool   result  = false;
};

class ComparisonLessThanFactory : public BlockFactory {
  Q_OBJECT

public:
  ComparisonLessThanFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ComparisonLessThan" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Comparison A < B" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Comparison" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
