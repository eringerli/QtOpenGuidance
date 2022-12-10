// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ComparisonGreaterOrEqualTo : public BlockBase {
  Q_OBJECT

public:
  explicit ComparisonGreaterOrEqualTo() : BlockBase() {}

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

class ComparisonGreaterOrEqualToFactory : public BlockFactory {
  Q_OBJECT

public:
  ComparisonGreaterOrEqualToFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ComparisonGreaterOrEqualTo" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Comparison A >= B" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Comparison" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
