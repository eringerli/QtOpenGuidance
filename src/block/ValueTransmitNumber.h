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

class ValueTransmitNumber : public ValueTransmitBase {
    Q_OBJECT
  public:
    explicit ValueTransmitNumber( int id ) : ValueTransmitBase( id ) {}

  public slots:
    void setNumber( const double number ) {
      QJsonObject jsonObject;
      setChannelIdToJson( jsonObject );
      jsonObject[QStringLiteral( "number" )] = number;

      QJsonDocument jsonDoc;
      jsonDoc.setObject( jsonObject );

      emit dataToSend( jsonDoc.toJson() );
    }

    void dataReceive( const QByteArray& data ) {
      QJsonDocument jsonDoc( QJsonDocument::fromJson( data ) );
      QJsonObject jsonObject = jsonDoc.object();

      if( jsonObject[QStringLiteral( "channelId" )] == id ) {
        emit numberChanged( jsonObject[QStringLiteral( "number" )].toDouble( qInf() ) );
      }
    }

  signals:
    void dataToSend( const QByteArray& );
    void numberChanged( const double );
};

class ValueTransmitNumberFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmitNumberFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Number" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ValueTransmitNumber( id );
      auto* b = createBaseBlock( scene, obj, id, false, QNEBlock::Flag::Normal | QNEBlock::Flag::Embedded );

      b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

      b->addInputPort( QStringLiteral( "Number" ), QLatin1String( SLOT( setNumber( const double ) ) ), false );
      b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( numberChanged( const double ) ) ), true );

      b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );

      b->addInputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( numberChanged( const double ) ) ), true );
      b->addOutputPort( QStringLiteral( "Number" ), QLatin1String( SIGNAL( numberChanged( const double ) ) ), false );

      return b;
    }
};
