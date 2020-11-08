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

#include <Eigen/Geometry>

#include "ValueTransmissionBase.h"

class ValueTransmissionQuaternion : public ValueTransmissionBase {
    Q_OBJECT
  public:
    explicit ValueTransmissionQuaternion( int id ) : ValueTransmissionBase( id ) {}

  public slots:
    void setQuaternion( const Eigen::Quaterniond quaternion ) {
      QCborMap map;
      map[QStringLiteral( "channelId" )] = id;
      map[QStringLiteral( "x" )] = quaternion.x();
      map[QStringLiteral( "y" )] = quaternion.y();
      map[QStringLiteral( "z" )] = quaternion.z();
      map[QStringLiteral( "w" )] = quaternion.w();

      emit dataToSend( QCborValue( std::move( map ) ).toCbor() );
    }

    void dataReceive( const QByteArray& data ) {
      reader.addData( data );

      auto cbor = QCborValue::fromCbor( reader );

      if( cbor.isMap() && ( cbor[QStringLiteral( "channelId" )] == id ) ) {

        auto x = cbor[QStringLiteral( "x" )].toDouble( 0 );
        auto y = cbor[QStringLiteral( "y" )].toDouble( 0 );
        auto z = cbor[QStringLiteral( "z" )].toDouble( 0 );
        auto w = cbor[QStringLiteral( "w" )].toDouble( 0 );

        emit quaternionChanged( Eigen::Quaterniond( w, x, y, z ) );
      }
    }

  signals:
    void dataToSend( const QByteArray& );
    void quaternionChanged( const Eigen::Quaterniond );

  private:
    QCborStreamReader reader;
};

class ValueTransmissionQuaternionFactory : public BlockFactory {
    Q_OBJECT

  public:
    ValueTransmissionQuaternionFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Value Transmit Quaternion" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Value Converters" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new ValueTransmissionQuaternion( id );
      auto* b = createBaseBlock( scene, obj, id, false );

      b->addInputPort( QStringLiteral( "CBOR In" ), QLatin1String( SLOT( dataReceive( const QByteArray& ) ) ) );
      b->addOutputPort( QStringLiteral( "Out" ), QLatin1String( SIGNAL( quaternionChanged( const Eigen::Quaterniond ) ) ), false );

      b->addInputPort( QStringLiteral( "In" ), QLatin1String( SLOT( setQuaternion( const Eigen::Quaterniond ) ) ), false );
      b->addOutputPort( QStringLiteral( "CBOR Out" ), QLatin1String( SIGNAL( dataToSend( const QByteArray& ) ) ), false );

      b->setBrush( converterColor );

      return b;
    }
};
