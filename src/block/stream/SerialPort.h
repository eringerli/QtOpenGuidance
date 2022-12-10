// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class QSerialPort;

class SerialPort : public BlockBase {
  Q_OBJECT

public:
  explicit SerialPort();
  ~SerialPort();

signals:
  void dataReceived( const QByteArray& );

public slots:
  void setPort( const QString& port );

  void setBaudrate( NUMBER_SIGNATURE_SLOT );

  void sendData( const QByteArray& data );

protected slots:
  void processPendingData();

public:
  QString port;
  double  baudrate = 0;

private:
  QSerialPort* serialPort = nullptr;
};

class SerialPortFactory : public BlockFactory {
  Q_OBJECT

public:
  SerialPortFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Serial Port" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Streams" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
