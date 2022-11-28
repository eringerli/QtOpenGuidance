// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SerialPort.h"

#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QByteArray>
#include <QObject>
#include <QSerialPort>

SerialPort::SerialPort() {
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

QNEBlock*
SerialPortFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SerialPort();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Port" ), QLatin1String( SLOT( setPort( QString ) ) ) );
  b->addInputPort( QStringLiteral( "Baudrate" ), QLatin1String( SLOT( setBaudrate( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( inputOutputColor );

  return b;
}
