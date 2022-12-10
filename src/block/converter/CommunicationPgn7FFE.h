// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class CommunicationPgn7ffe : public BlockBase {
  Q_OBJECT

public:
  explicit CommunicationPgn7ffe() : BlockBase() {}

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setXte( NUMBER_SIGNATURE_SLOT );

  void setVelocity( NUMBER_SIGNATURE_SLOT );

private:
  float distance = 0;
  float velocity = 0;
};

class CommunicationPgn7ffeFactory : public BlockFactory {
  Q_OBJECT

public:
  CommunicationPgn7ffeFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Communication PGN 7FFE" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Legacy Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
