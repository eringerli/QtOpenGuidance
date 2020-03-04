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

class ValueTransmissionNumber : public ValueTransmissionBase {
    Q_OBJECT
  public:
    explicit ValueTransmissionNumber( int id ) : ValueTransmissionBase( id ) {}

  public slots:
    void setNumber( const double number ) {
      QCborMap map;
      map[QStringLiteral( "channelId" )] = id;
      map[QStringLiteral( "number" )] = number;

      emit dataToSend( QCborValue( std::move( map ) ).toCbor() );
    }

    void setNumberEmbedded( EmbeddedBlockDummy, const double ) {}

    void dataReceive( const QByteArray& data ) {
      reader.addData( data );

      auto cbor = QCborValue::fromCbor( reader );

      if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {
        emit numberChanged( cbor[QStringLiteral( "number" )].toDouble( 0 ) );
      }
    }

  signals:
    void dataToSend( const QByteArray& );
    void numberChanged( const double );
    void numberChangedEmbedded( EmbeddedBlockDummy, const double );

  private:
    QCborStreamReader reader;
};

class ValueTransmissionNumberFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmissionNumberFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Number" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ValueTransmissionNumber( id );
      auto* b = createBaseBlock( scene, obj, id, false, QNEBlock::Flag::Normal | QNEBlock::Flag::Embedded );

      b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
//      b->addInputPort( QStringLiteral( "Embedded In" ), QLatin1String( SLOT( setNumberEmbedded( EmbeddedBlockDummy, const double ) ) ), true );
      b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( numberChanged( const double ) ) ), false );

      b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setNumber( const double ) ) ), false );
      b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );
//      b->addOutputPort( QStringLiteral( "Embedded Out" ), QLatin1String( SIGNAL( numberChangedEmbedded( EmbeddedBlockDummy, const double ) ) ), true );

      return b;
    }
};
