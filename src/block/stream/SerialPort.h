// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class QSerialPort;

class SerialPort : public BlockBase {
  Q_OBJECT

public:
  explicit SerialPort( const BlockBaseId idHint, const bool systemBlock, const QString type );
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
  SerialPortFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::InputOutput; }

  QString getNameOfFactory() const override { return QStringLiteral( "Serial Port" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Streams" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
