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

#include "GuidanceBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

#include <GeographicLib/TransverseMercator.hpp>
using namespace std;
using namespace GeographicLib;

class TransverseMercatorConverter : public GuidanceBase {
    Q_OBJECT

  public:
    explicit TransverseMercatorConverter( Tile* tile, bool* isLatLonOffsetSet, double* height0, double* lat0, double* lon0 )
      : GuidanceBase(),
        _tm( Constants::WGS84_a(), Constants::WGS84_f(), Constants::UTM_k0() ),
        isLatLonOffsetSet( isLatLonOffsetSet ), height0( height0 ), lat0( lat0 ), lon0( lon0 ) {
      rootTile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setWGS84Position( double latitude, double longitude, double height ) {
      if( !( *isLatLonOffsetSet ) ) {
        *lat0 = latitude;
        *lon0 = longitude;
        *height0 = height;
        *isLatLonOffsetSet = true;
      }

      latitude -= *lat0;
      height -= *height0;

      double x, y, convergence, scale;
      _tm.Forward( *lon0, latitude, longitude, y, x, convergence, scale );

//      qDebug() << x << y << convergence << scale;
//      qDebug() << *lat0 << *lon0;

      Tile* tile = rootTile->getTileForPosition( x, y );
//      qDebug() << tile << "(" << tile->x << "|" << tile->y << ")" << x << y;

      emit tiledPositionChanged( tile, QVector3D( float( x ), float( y ), float( height ) ) );
    }

  signals:
    void tiledPositionChanged( Tile* tile, QVector3D position );

  public:
    virtual void emitConfigSignals() override {
      rootTile = rootTile->getTileForPosition( &position );
      emit tiledPositionChanged( rootTile, position );
    }

  public:
    Tile* rootTile = nullptr;
    QVector3D position = QVector3D();

    TransverseMercator _tm;

    bool* isLatLonOffsetSet;
    double* height0;
    double* lat0;
    double* lon0;
};

class TransverseMercatorConverterFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    TransverseMercatorConverterFactory( Tile* tile )
      : GuidanceFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Transverse Mercator" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      RootTile* rootTile = qobject_cast<RootTile*>( tile->parent() );

      return new TransverseMercatorConverter( tile, &( rootTile->isLatLonOffsetSet ), &( rootTile->height0 ), &( rootTile->lat0 ), &( rootTile->lon0 ) );
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
};

#endif // TRANSVERSEMERCATORCONVERTER_H

