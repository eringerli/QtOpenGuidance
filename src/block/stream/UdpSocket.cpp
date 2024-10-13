// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "UdpSocket.h"

#include <QtNetwork>

UdpSocket::UdpSocket( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {
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

std::unique_ptr< BlockBase >
UdpSocketFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< UdpSocket >( idHint );

  obj->addInputPort( QStringLiteral( "Port" ), obj.get(), QLatin1StringView( SLOT( setPort( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SLOT( sendData( const QByteArray& ) ) ) );

  obj->addOutputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SIGNAL( dataReceived( const QByteArray& ) ) ) );

  return obj;
}
