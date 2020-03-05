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

#include "BlockBase.h"

class CommunicationPgn7ffe : public BlockBase {
    Q_OBJECT

  public:
    explicit CommunicationPgn7ffe()
      : BlockBase() {
    }

  signals:
    void  dataReceived( const QByteArray& );

  public slots:
    void setSteeringAngle( double steeringAngle ) {
      QByteArray data;
      data.resize( 8 );
      data[0] = char( 0x7f );
      data[1] = char( 0xfe );

      // relais
      data[2] = 0;

      // velocity in km/4h
      data[3] = char( velocity * 3.6f * 4.0f );

      // XTE in mm
      auto xte = int16_t( distance * 1000 );
      data[4] = char( xte >> 8 );
      data[5] = char( xte & 0xff );

      // steerangle in °/100
      auto steerangle = int16_t( steeringAngle * 100 );
      data[6] = char( steerangle >> 8 );
      data[7] = char( steerangle & 0xff );

      emit dataReceived( data );
    }

    void setXte( double distance ) {
      this->distance = distance;
    }

    void setVelocity( double velocity ) {
      this->velocity = velocity;
    }

  private:
    float distance = 0;
    float velocity = 0;
};

class CommunicationPgn7ffeFactory : public BlockFactory {
    Q_OBJECT

  public:
    CommunicationPgn7ffeFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Communication PGN 7FFE" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new CommunicationPgn7ffe();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( double ) ) ) );
      b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

      b->setBrush( QColor( QStringLiteral( "mediumaquamarine" ) ) );

      return b;
    }
};

