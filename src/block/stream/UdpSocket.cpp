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

#include "UdpSocket.h"

#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QtNetwork>

UdpSocket::UdpSocket() {
  udpSocket = new QUdpSocket( this );
  connect( udpSocket, &QIODevice::readyRead,
           this, &UdpSocket::processPendingDatagrams );
}

UdpSocket::~UdpSocket() {
  if( udpSocket ) {
    udpSocket->deleteLater();
  }
}

void UdpSocket::setPort( double port ) {
  this->port = port;
  udpSocket->bind( quint16( port ),  QUdpSocket::DontShareAddress );
}

void UdpSocket::sendData( const QByteArray& data ) {
  udpSocket->writeDatagram( data, QHostAddress::Broadcast, port );
}

void UdpSocket::processPendingDatagrams() {
  QByteArray datagram;

  while( udpSocket->hasPendingDatagrams() ) {
    datagram.resize( int( udpSocket->pendingDatagramSize() ) );
    udpSocket->readDatagram( datagram.data(), datagram.size() );
    Q_EMIT dataReceived( datagram );
  }
}

QNEBlock* UdpSocketFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new UdpSocket();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Port" ), QLatin1String( SLOT( setPort( double ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( inputOutputColor );

  return b;
}
