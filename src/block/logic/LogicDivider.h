// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class LogicDivider : public BlockBase {
  Q_OBJECT

public:
  explicit LogicDivider( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setSwitch( ACTION_SIGNATURE );
  void setValueA( NUMBER_SIGNATURE_SLOT );
  void setValueB( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  bool switchValue = false;
};

class LogicDividerFactory : public BlockFactory {
  Q_OBJECT

public:
  LogicDividerFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "LogicDivider" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Logic Divider" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Logic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
