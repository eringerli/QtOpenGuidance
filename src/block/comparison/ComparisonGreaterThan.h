// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ComparisonGreaterThan : public BlockBase {
  Q_OBJECT

public:
  explicit ComparisonGreaterThan() : BlockBase() {}

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

class ComparisonGreaterThanFactory : public BlockFactory {
  Q_OBJECT

public:
  ComparisonGreaterThanFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ComparisonGreaterThan" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Comparison A > B" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Comparison" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
