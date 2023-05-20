// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class StringBlockModel;

class StringObject : public BlockBase {
  Q_OBJECT

public:
  explicit StringObject() : BlockBase() {}

  void emitConfigSignals() override;

  QJsonObject toJSON() const override;
  void        fromJSON( QJsonObject& ) override;

Q_SIGNALS:
  void stringChanged( const QString& );

public:
  QString string;
};

class StringFactory : public BlockFactory {
  Q_OBJECT

public:
  StringFactory( QThread* thread, StringBlockModel* model ) : BlockFactory( thread, false ), model( model ) {}

  QString getNameOfFactory() override { return QStringLiteral( "String" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Literals" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  StringBlockModel* model = nullptr;
};
