// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class NumberBlockModel;

class NumberObject : public BlockBase {
  Q_OBJECT

public:
  explicit NumberObject() : BlockBase() {}

  void emitConfigSignals() override;

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  double number = 0;
};

class NumberFactory : public BlockFactory {
  Q_OBJECT

public:
  NumberFactory( QThread* thread, NumberBlockModel* model ) : BlockFactory( thread ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Number" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  NumberBlockModel* model = nullptr;
};
