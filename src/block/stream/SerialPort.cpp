// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SerialPort.h"

#include <QByteArray>
#include <QObject>
#include <QSerialPort>

SerialPort::SerialPort( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {
  serialPort = new QSerialPort( this );
  connect( serialPort, &QIODevice::readyRead, this, &SerialPort::processPendingData );
}

SerialPort::~SerialPort() {
  if( serialPort ) {
    serialPort->deleteLater();
  }
}

void
SerialPort::setPort( const QString& port ) {
  this->port = port;
  serialPort->close();
  serialPort->setPortName( port );
  serialPort->open( QIODevice::ReadWrite );
}

void
SerialPort::setBaudrate( const double baudrate, CalculationOption::Options ) {
  this->baudrate = baudrate;
  serialPort->setBaudRate( qint32( baudrate ) );
}

void
SerialPort::sendData( const QByteArray& data ) {
  serialPort->write( data );
}

void
SerialPort::processPendingData() {
  QByteArray datagram;

  while( serialPort->bytesAvailable() ) {
    datagram.resize( int( serialPort->bytesAvailable() ) );
    serialPort->read( datagram.data(), datagram.size() );
    Q_EMIT dataReceived( datagram );
  }
}

std::unique_ptr< BlockBase >
SerialPortFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< SerialPort >( idHint );

  obj->addInputPort( QStringLiteral( "Port" ), obj.get(), QLatin1StringView( SLOT( setPort( QString ) ) ) );
  obj->addInputPort( QStringLiteral( "Baudrate" ), obj.get(), QLatin1StringView( SLOT( setBaudrate( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SLOT( sendData( const QByteArray& ) ) ) );

  obj->addOutputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  return obj;
}
