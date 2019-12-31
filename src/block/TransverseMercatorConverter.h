// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef TRANSVERSEMERCATORCONVERTER_H
#define TRANSVERSEMERCATORCONVERTER_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/TransverseMercatorWrapper.h"

#include <QDebug>

class TransverseMercatorConverter : public BlockBase {
    Q_OBJECT

  public:
    explicit TransverseMercatorConverter( TransverseMercatorWrapper* tmw )
      : BlockBase(),
        tmw( tmw ) {}

  public slots:
    void setWGS84Position( double latitude, double longitude, double height ) {

      double x;
      double y;
      tmw->Forward( latitude, longitude, height, x, y );

      emit positionChanged( Point_3( x, y, height ) );
    }

  signals:
    void positionChanged( Point_3 position );

  public:
    virtual void emitConfigSignals() override {
      emit positionChanged( Point_3() );
    }

  public:
    TransverseMercatorWrapper* tmw = nullptr;
};

class TransverseMercatorConverterFactory : public BlockFactory {
    Q_OBJECT

  public:
    TransverseMercatorConverterFactory( TransverseMercatorWrapper* tmw )
      : BlockFactory(),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Transverse Mercator" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new TransverseMercatorConverter( tmw );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "WGS84 Position", SLOT( setWGS84Position( double, double, double ) ) );

      b->addOutputPort( "Position", SIGNAL( positionChanged( Point_3 ) ) );

      return b;
    }

  private:
    TransverseMercatorWrapper* tmw = nullptr;
};

#endif // TRANSVERSEMERCATORCONVERTER_H
