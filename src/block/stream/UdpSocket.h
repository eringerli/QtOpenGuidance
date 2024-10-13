// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class QUdpSocket;

class UdpSocket : public BlockBase {
  Q_OBJECT

public:
  explicit UdpSocket( const BlockBaseId idHint, const bool systemBlock, const QString type );

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
  UdpSocketFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::InputOutput; }

  QString getNameOfFactory() const override { return QStringLiteral( "UDP Socket" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Streams" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
