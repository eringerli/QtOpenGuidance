// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ValveNumber : public BlockBase {
  Q_OBJECT

public:
  explicit ValveNumber( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

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

class ValveNumberFactory : public BlockFactory {
  Q_OBJECT

public:
  ValveNumberFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "ValveNumber" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Valve for Numbers" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Logic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
