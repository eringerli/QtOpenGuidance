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

#pragma once

#include <QObject>
#include <QByteArray>
#include <QSerialPort>

#include "BlockBase.h"

class SerialPort : public BlockBase {
    Q_OBJECT

  public:
    explicit SerialPort()
      : BlockBase() {
      serialPort = new QSerialPort( this );
      connect( serialPort, &QIODevice::readyRead,
               this, &SerialPort::processPendingData );
    }

    ~SerialPort() {
      if( serialPort ) {
        serialPort->deleteLater();
      }
    }

    void emitConfigSignals() override {
    }

  signals:
    void  dataReceived( const QByteArray& );

  public slots:
    void setPort( const QString& port ) {
      this->port = port;
      serialPort->close();
      serialPort->setPortName( port );
      serialPort->open( QIODevice::ReadWrite );
    }

    void setBaudrate( double baudrate ) {
      this->baudrate = baudrate;
      serialPort->setBaudRate( qint32( baudrate ) );
    }

    void sendData( const QByteArray& data ) {
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

class SerialPortFactory : public BlockFactory {
    Q_OBJECT

  public:
    SerialPortFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Serial Port" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new SerialPort();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Port" ), QLatin1String( SLOT( setPort( QString ) ) ) );
      b->addInputPort( QStringLiteral( "Baudrate" ), QLatin1String( SLOT( setBaudrate( double ) ) ) );
      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

      b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

      b->setBrush( QColor( QStringLiteral( "cornflowerblue" ) ) );

      return b;
    }
};
