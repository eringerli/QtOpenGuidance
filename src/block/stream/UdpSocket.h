// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class QUdpSocket;

class UdpSocket : public BlockBase {
  Q_OBJECT

public:
  explicit UdpSocket();

  ~UdpSocket();

Q_SIGNALS:
  void dataReceived( const QByteArray& );

public Q_SLOTS:
  void setPort( NUMBER_SIGNATURE_SLOT );

  void sendData( const QByteArray& data );

protected Q_SLOTS:
  void processPendingDatagrams();

public:
  float port = 0;

private:
  QUdpSocket* udpSocket = nullptr;
};

class UdpSocketFactory : public BlockFactory {
  Q_OBJECT

public:
  UdpSocketFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "UDP Socket" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Streams" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
