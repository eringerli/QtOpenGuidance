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

#include <QJsonDocument>
#include <QJsonObject>

#include "ValueTransmissionBase.h"

class ValueTransmissionBase64Data : public ValueTransmissionBase {
    Q_OBJECT
  public:
    explicit ValueTransmissionBase64Data( int id ) : ValueTransmissionBase( id ) {}

  public slots:
    void setData( const QByteArray& data ) {
      QCborMap map;
      map[QStringLiteral( "channelId" )] = id;
      map[QStringLiteral( "data" )] = QString( data.toBase64( QByteArray::OmitTrailingEquals ) );

      emit dataToSend( QCborValue( std::move( map ) ).toCbor() );
    }

    void dataReceive( const QByteArray& data ) {
      reader.addData( data );

      auto cbor = QCborValue::fromCbor( reader );

      if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {
        emit dataChanged( QByteArray::fromBase64( cbor[QStringLiteral( "data" )].toString().toLatin1() ) );
      }
    }

  signals:
    void dataToSend( const QByteArray& );
    void dataChanged( const QByteArray& );

  private:
    QCborStreamReader reader;
};

class ValueTransmissionBase64DataFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmissionBase64DataFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Base64 Data" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ValueTransmissionBase64Data( id );
      auto* b = createBaseBlock( scene, obj, id, false );

      b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
      b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( dataChanged( const QByteArray& ) ) ), false );

      b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ), false );
      b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

      b->setBrush( converterColor );

      return b;
    }
};
