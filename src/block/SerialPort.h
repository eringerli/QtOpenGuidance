// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QByteArray>
#include <QSerialPort>

#include "GuidanceBase.h"

class SerialPort : public GuidanceBase {
    Q_OBJECT

  public:
    explicit SerialPort()
      : GuidanceBase() {
      serialPort = new QSerialPort( this );
      connect( serialPort, SIGNAL( readyRead() ),
               this, SLOT( processPendingData() ) );
    }

    void emitConfigSignals() override {
    }

  signals:
    void dataReceived( QByteArray );

  public slots:
    void setPort( QString port ) {
      this->port = port;
      serialPort->close();
      serialPort->setPortName( port );
      serialPort->open( QIODevice::ReadWrite );
    }

    void setBaudrate( float baudrate ) {
      this->baudrate = baudrate;
      serialPort->setBaudRate( qint32( baudrate ) );
    }

    void sendData( QByteArray data ) {
      serialPort->write( data );
    }

  protected slots:
    void processPendingData() {
      QByteArray datagram;

      while( serialPort->bytesAvailable() ) {
        datagram.resize( int( serialPort->bytesAvailable() ) );
        serialPort->read( datagram.data(), datagram.size() );
        emit dataReceived( datagram );
      }
    }

  public:
    QString port;
    float baudrate = 0;

  private:
    QSerialPort* serialPort = nullptr;
};

class SerialPortFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    SerialPortFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Serial Port" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new SerialPort();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Port", SLOT( setPort( QString ) ) );
      b->addInputPort( "Baudrate", SLOT( setBaudrate( float ) ) );
      b->addInputPort( "Data", SLOT( sendData( QByteArray ) ) );

      b->addOutputPort( "Data", SIGNAL( dataReceived( QByteArray ) ) );

      return b;
    }
};

#endif // SERIALPORT_H
