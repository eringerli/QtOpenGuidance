// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class CommunicationJrk : public BlockBase {
  Q_OBJECT

public:
  explicit CommunicationJrk() : BlockBase() {}

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setSteerZero( NUMBER_SIGNATURE_SLOT );

  void setSteerCountPerDegree( NUMBER_SIGNATURE_SLOT );

private:
  float steerZero       = 2047;
  float countsPerDegree = 45;
};

class CommunicationJrkFactory : public BlockFactory {
  Q_OBJECT

public:
  CommunicationJrkFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Communication JRK" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Legacy Converters" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
