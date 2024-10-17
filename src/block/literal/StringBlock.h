// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class StringBlockModel;

class StringBlock : public BlockBase {
  Q_OBJECT

public:
  explicit StringBlock( const BlockBaseId idHint, const bool systemBlock, const QString type, const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

  void emitConfigSignals() override;

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

Q_SIGNALS:
  void stringChanged( const QString& );

public:
  QString string;
};

class StringBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  StringBlockFactory( QThread* thread, StringBlockModel* model ) : BlockFactory( thread, false ), model( model ) {
    typeColor = BlockBase::TypeColor::Value;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "String" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Literals" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  StringBlockModel* model = nullptr;
};
