// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "SerialPort.h"

#include "qneblock.h"
#include "qneport.h"

#include <QObject>
#include <QBrush>
#include <QByteArray>
#include <QSerialPort>

SerialPort::SerialPort() {
  serialPort = new QSerialPort( this );
  connect( serialPort, &QIODevice::readyRead,
           this, &SerialPort::processPendingData );
}

SerialPort::~SerialPort() {
  if( serialPort ) {
    serialPort->deleteLater();
  }
}

void SerialPort::setPort( const QString& port ) {
  this->port = port;
  serialPort->close();
  serialPort->setPortName( port );
  serialPort->open( QIODevice::ReadWrite );
}

void SerialPort::setBaudrate( double baudrate ) {
  this->baudrate = baudrate;
  serialPort->setBaudRate( qint32( baudrate ) );
}

void SerialPort::sendData( const QByteArray& data ) {
  serialPort->write( data );
}

void SerialPort::processPendingData() {
  QByteArray datagram;

  while( serialPort->bytesAvailable() ) {
    datagram.resize( int( serialPort->bytesAvailable() ) );
    serialPort->read( datagram.data(), datagram.size() );
    Q_EMIT dataReceived( datagram );
  }
}

QNEBlock* SerialPortFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new SerialPort();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Port" ), QLatin1String( SLOT( setPort( QString ) ) ) );
  b->addInputPort( QStringLiteral( "Baudrate" ), QLatin1String( SLOT( setBaudrate( double ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( inputOutputColor );

  return b;
}
