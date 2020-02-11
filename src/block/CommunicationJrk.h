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

#ifndef COMMUNICATIONJRK_H
#define COMMUNICATIONJRK_H

#include <QObject>
#include <QByteArray>

#include "BlockBase.h"

class CommunicationJrk : public BlockBase {
    Q_OBJECT

  public:
    explicit CommunicationJrk()
      : BlockBase() {
    }

  signals:
    void  dataReceived( const QByteArray& );

  public slots:
    void setSteeringAngle( float steeringAngle ) {

      QByteArray data;
      data.resize( 2 );

      int16_t target = int16_t( steeringAngle * countsPerDegree ) + int16_t( steerZero );
      data[0] = char( 0xc0 | ( target & 0x1f ) );
      data[1] = char( ( target >> 5 ) & 0x7f );

      emit dataReceived( data );
    }

    void setSteerZero( float steerZero ) {
      this->steerZero = steerZero;
    }

    void setSteerCountPerDegree( float countsPerDegree ) {
      this->countsPerDegree = countsPerDegree;
    }

  private:
    float steerZero = 2047;
    float countsPerDegree = 45;
};

class CommunicationJrkFactory : public BlockFactory {
    Q_OBJECT

  public:
    CommunicationJrkFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Communication JRK" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new CommunicationJrk();
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "Steerzero" ), QLatin1String( SLOT( setSteerZero( float ) ) ) );
      b->addInputPort( QStringLiteral( "Steering count/°" ), QLatin1String( SLOT( setSteerCountPerDegree( float ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( float ) ) ) );
      b->addOutputPort( QStringLiteral( "Data" ), QLatin1String( SIGNAL( dataReceived( const QByteArray& ) ) ) );

      return b;
    }
};

#endif // COMMUNICATIONJRK_H
