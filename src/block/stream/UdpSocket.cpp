// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "UdpSocket.h"

#include "qneblock.h"
#include "qneport.h"

#include <QBrush>
#include <QtNetwork>

UdpSocket::UdpSocket() {
  udpSocket = new QUdpSocket( this );
  connect( udpSocket, &QIODevice::readyRead, this, &UdpSocket::processPendingDatagrams );
}

UdpSocket::~UdpSocket() {
  if( udpSocket ) {
    udpSocket->deleteLater();
  }
}

void
UdpSocket::setPort( double port, const CalculationOption::Options ) {
  this->port = port;
  udpSocket->bind( quint16( port ), QUdpSocket::DontShareAddress );
}

void
UdpSocket::sendData( const QByteArray& data ) {
  udpSocket->writeDatagram( data, QHostAddress::Broadcast, port );
}

void
UdpSocket::processPendingDatagrams() {
  QByteArray datagram;

  while( udpSocket->hasPendingDatagrams() ) {
    datagram.resize( int( udpSocket->pendingDatagramSize() ) );
    udpSocket->readDatagram( datagram.data(), datagram.size() );
    Q_EMIT dataReceived( datagram );
  }
}

QNEBlock*
UdpSocketFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new UdpSocket();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Port" ), QLatin1String( SLOT( setPort( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( sendData( const QByteArray& ) ) ) );

  b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  b->setBrush( inputOutputColor );

  return b;
}
