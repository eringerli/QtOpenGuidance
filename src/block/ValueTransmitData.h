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

#include "ValueTransmitBase.h"

class ValueTransmitData : public ValueTransmitBase {
    Q_OBJECT
  public:
    explicit ValueTransmitData( int id ) : ValueTransmitBase( id ) {}

  public slots:
    void setData( const QByteArray& data ) {
      QCborMap map;
      map[QStringLiteral( "channelId" )] = id;
      map[QStringLiteral( "data" )] = data;

      emit dataToSend( QCborValue( std::move( map ) ).toCbor() );
    }

    void dataReceive( const QByteArray& data ) {
      reader.addData( data );

      auto cbor = QCborValue::fromCbor( reader );

      if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {
        emit dataChanged( cbor[QStringLiteral( "data" )].toByteArray() );
      }
    }

  signals:
    void dataToSend( const QByteArray& );
    void dataChanged( const QByteArray& );

  private:
    QCborStreamReader reader;
};

class ValueTransmitDataFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmitDataFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Data" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ValueTransmitData( id );
      auto* b = createBaseBlock( scene, obj, id, false, QNEBlock::Flag::Normal | QNEBlock::Flag::Embedded );

      b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ), false );
      b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );
      b->addOutputPort( QStringLiteral( "Embedded Out" ), QLatin1String( SIGNAL( dataChanged( const QByteArray& ) ) ), true );

      b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
      b->addInputPort( QStringLiteral( "Embedded In" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ), true );
      b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( dataChanged( const QByteArray& ) ) ), false );

      return b;
    }
};
