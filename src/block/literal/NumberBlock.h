// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class NumberBlockModel;

class NumberBlock : public BlockBase {
  Q_OBJECT

public:
  explicit NumberBlock( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  double number = 0;
};

class NumberBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  NumberBlockFactory( QThread* thread, NumberBlockModel* model ) : BlockFactory( thread, false ), model( model ) {
    typeColor = TypeColor::Value;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Number" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Literals" ); }

  virtual std::unique_ptr< BlockBase > createBlock( BlockBaseId idHint = 0 ) override;

private:
  NumberBlockModel* model = nullptr;
};
