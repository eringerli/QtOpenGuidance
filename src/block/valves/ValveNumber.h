// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class ValveNumber : public BlockBase {
  Q_OBJECT

public:
  explicit ValveNumber() : BlockBase() {}

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
  ValveNumberFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ValveNumber" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Valve for Numbers" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Valves" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};