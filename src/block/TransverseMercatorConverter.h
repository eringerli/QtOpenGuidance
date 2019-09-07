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

#include "../kinematic/Tile.h"

#include <QDebug>

#include <GeographicLib/TransverseMercator.hpp>

using namespace std;
using namespace GeographicLib;

// an instance of this class gets shared across all the blocks, so the conversions are the same everywhere
class TransverseMercatorWrapper {
  public:

    TransverseMercatorWrapper()
      : _tm( Constants::WGS84_a(), Constants::WGS84_f(), Constants::UTM_k0() ) {
    }

    void Forward( double latitude, double longitude, double& height, double& x, double& y ) {
      if( !isLatLonOffsetSet ) {
        lat0 = latitude;
        lon0 = longitude;
        height0 = height;
        isLatLonOffsetSet = true;
      }

      latitude -= lat0;
      height -= height0;

      double convergence, scale;
      _tm.Forward( lon0, latitude, longitude, y, x, convergence, scale );
//      qDebug() << "lat0, lon0, isLatLonOffsetSet" << lat0 << lon0 << isLatLonOffsetSet;
//      qDebug() << x << y << convergence << scale;
    }

    void Reverse( double x, double y, double& latitude, double& longitude, double& height ) {
      double convergence, scale;
      _tm.Reverse( lon0, y, x, latitude, longitude, convergence, scale );

      latitude += lat0;
      height += height0;
      qDebug() << lat0 << lon0;
      qDebug() << x << y << convergence << scale;
    }

  private:
    TransverseMercator _tm;

    bool isLatLonOffsetSet = false;
    double height0 = 0;
    double lat0 = 0;
    double lon0 = 0;
};

class TransverseMercatorConverter : public BlockBase {
    Q_OBJECT

  public:
    explicit TransverseMercatorConverter( Tile* tile, TransverseMercatorWrapper* tmw )
      : BlockBase(),
        tmw( tmw ) {
      rootTile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setWGS84Position( double latitude, double longitude, double height ) {

      double x, y;
      tmw->Forward( latitude, longitude, height, x, y );

      Tile* tile = rootTile->getTileForPosition( x, y );

      emit tiledPositionChanged( tile, QVector3D( float( x ), float( y ), float( height ) ) );
    }

  signals:
    void tiledPositionChanged( Tile* tile, QVector3D position );

  public:
    virtual void emitConfigSignals() override {
      emit tiledPositionChanged( rootTile, position );
    }

  public:
    Tile* rootTile = nullptr;
    TransverseMercatorWrapper* tmw = nullptr;
    QVector3D position = QVector3D();


};

class TransverseMercatorConverterFactory : public BlockFactory {
    Q_OBJECT

  public:
    TransverseMercatorConverterFactory( Tile* tile, TransverseMercatorWrapper* tmw )
      : BlockFactory(),
        tile( tile ), tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Transverse Mercator" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new TransverseMercatorConverter( tile, tmw );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "WGS84 Position", SLOT( setWGS84Position( double, double, double ) ) );

      b->addOutputPort( "Tiled Position", SIGNAL( tiledPositionChanged( Tile*, QVector3D ) ) );

      return b;
    }

  private:
    Tile* tile;
    TransverseMercatorWrapper* tmw;
};

#endif // TRANSVERSEMERCATORCONVERTER_H

