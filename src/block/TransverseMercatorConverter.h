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

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/GeographicConvertionWrapper.h"

#include <QDebug>

class TransverseMercatorConverter : public BlockBase {
    Q_OBJECT

  public:
    explicit TransverseMercatorConverter( GeographicConvertionWrapper* tmw )
      : BlockBase(),
        tmw( tmw ) {}

  public slots:
    void setWGS84Position( const double latitude, const double longitude, const double height ) {
      double x = 0;
      double y = 0;
      double z = 0;
      tmw->Forward( latitude, longitude, height, x, y, z );

      auto point = Point_3( x, y, z );
      emit positionChanged( point );
    }

  signals:
    void positionChanged( const Point_3& );

  public:
    virtual void emitConfigSignals() override {
      auto dummyPoint = Point_3();
      emit positionChanged( dummyPoint );
    }

  public:
    GeographicConvertionWrapper* tmw = nullptr;
};

class TransverseMercatorConverterFactory : public BlockFactory {
    Q_OBJECT

  public:
    TransverseMercatorConverterFactory( GeographicConvertionWrapper* tmw )
      : BlockFactory(),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Transverse Mercator" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new TransverseMercatorConverter( tmw );
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( const double, const double, const double ) ) ) );

      b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( const Point_3& ) ) ) );

      return b;
    }

  private:
    GeographicConvertionWrapper* tmw = nullptr;
};

