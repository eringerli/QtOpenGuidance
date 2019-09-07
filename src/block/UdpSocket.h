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

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QtNetwork>

#include "BlockBase.h"

class UdpSocket : public BlockBase {
    Q_OBJECT

  public:
    explicit UdpSocket()
      : BlockBase() {
      udpSocket = new QUdpSocket( this );
      connect( udpSocket, SIGNAL( readyRead() ),
               this, SLOT( processPendingDatagrams() ) );
    }

    ~UdpSocket() {
      if( udpSocket ) {
        udpSocket->deleteLater();
      }
    }

    void emitConfigSignals() override {
    }

  signals:
    void dataReceived( QByteArray );

  public slots:
    void setPort( float port ) {
      this->port = port;
      udpSocket->bind( quint16( port ),  QUdpSocket::DontShareAddress );
    }

    void sendData( QByteArray data ) {
      udpSocket->writeDatagram( data, QHostAddress::Broadcast, port );
    }

  protected slots:
    void processPendingDatagrams() {
      QByteArray datagram;

      while( udpSocket->hasPendingDatagrams() ) {
        datagram.resize( int( udpSocket->pendingDatagramSize() ) );
        udpSocket->readDatagram( datagram.data(), datagram.size() );
        emit dataReceived( datagram );
      }
    }

  public:
    float port = 0;

  private:
    QUdpSocket* udpSocket = nullptr;
};

class UdpSocketFactory : public BlockFactory {
    Q_OBJECT

  public:
    UdpSocketFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "UDP Socket" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new UdpSocket();
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Port", SLOT( setPort( float ) ) );
      b->addInputPort( "Data", SLOT( sendData( QByteArray ) ) );

      b->addOutputPort( "Data", SIGNAL( dataReceived( QByteArray ) ) );

      return b;
    }
};

#endif // UDPSOCKET_H
