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

#ifndef TILE_H
#define TILE_H

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>

#include <QList>
#include <QVector>

#include <QRectF>
#include <QVector3D>
#include <QQuaternion>

#include <QtMath>
#include <QtGlobal>

#include "../3d/linemesh.h"
#include <Qt3DExtras/QPhongMaterial>

#include <QDebug>

// Tiling works like this:
// - create a QObject as root
// - add a first Tile to that QObject
// - all future Tiles have to add themselfs to this QObject, so they are in the children() of the root
//
// Tiles have a coordinate offset as an qint64, so the QVector3Ds can be used on these local coordinates, without
// loosing precision due to the limited digits in a float. Also there is an Qt3D-Entity with the transformation
// according to the offset, so all further manipulations can be done in local coordinates.

class Tile: public QObject {
    Q_OBJECT

  public:
    static constexpr float sizeOfTile = 50;
    static constexpr double sizeOfTileDouble = double( sizeOfTile );

  public:
    Tile( QObject* parent, qint64 x, qint64 y, Qt3DCore::QEntity* rootEntity, bool showTileEnabled, const QColor& color )
      : QObject( parent ),
        rootEntity( rootEntity ), x( x ), y( y ) {
      tileTransform = new Qt3DCore::QTransform();
      tileTransform->setTranslation( QVector3D( x, y, 0 ) );

      qDebug() << "new Tile:" << x << y << this;

      tileEntity = new Qt3DCore::QEntity( rootEntity );
      tileEntity->addComponent( tileTransform );

      limitsEntity = new Qt3DCore::QEntity( tileEntity );

      // comment in to draw lines around tile
      auto* linemesh = new LineMesh();
      QVector<QVector3D> lines;
      lines.append( QVector3D( 0, 0, 0 ) );
      lines.append( QVector3D( sizeOfTile, 0, 0 ) );
      lines.append( QVector3D( sizeOfTile, sizeOfTile, 0 ) );
      lines.append( QVector3D( 0, sizeOfTile, 0 ) );
      linemesh->posUpdate( lines );
      linemesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineLoop );

      this->color = color;
      this->showTileEnabled = showTileEnabled;

      material = new Qt3DExtras::QPhongMaterial();
      material->setAmbient( color );
      limitsEntity->addComponent( linemesh );
      limitsEntity->addComponent( material );

      limitsEntity->setParent( tileEntity );
      limitsEntity->setEnabled( showTileEnabled );
    }

    void setShowEnable( bool enabled ) {
      showTileEnabled = enabled;
      limitsEntity->setEnabled( enabled );
    }

    void setShowColor( const QColor& color ) {
      this->color = color;
      material->setAmbient( color );
    }

    // the check for locality is done here, as it is highly probable, that the point is in the current tile
    // this is an optimition, which isn't quite right from a point of ownership. Also, new Tiles are created here and
    // added to the parent of this Tile. This works, as the Tiles are organised as an object-tree (https://doc.qt.io/qt-5/objecttrees.html)
    // and manage themselfs
    // if a new tile is created, the position is altered accordingly
    Tile* getTileForPosition( double& x, double& y ) {
      // the point is in this Tile -> return it
      if( x >= 0 &&
          y >= 0 &&
          x < sizeOfTileDouble &&
          y < sizeOfTileDouble ) {
        return this;
      }

      // calculate relative offset
      qint64 relativeOffsetX = qint64( x / sizeOfTileDouble ) - ( ( x < 0 ) ? 1 : 0 );
      qint64 relativeOffsetY = qint64( y / sizeOfTileDouble ) - ( ( y < 0 ) ? 1 : 0 );
      relativeOffsetX *= qint64( sizeOfTileDouble );
      relativeOffsetY *= qint64( sizeOfTileDouble );

      // offset the position
      if( relativeOffsetX ) {
        x -= relativeOffsetX;
      }

      if( relativeOffsetY ) {
        y -= relativeOffsetY;
      }

      // get the tile or create a new one
      return getTileForOffset( this->x + relativeOffsetX, this->y + relativeOffsetY );

    }

    // the check for locality is done here, as it is highly probable, that the point is in the current tile
    // this is an optimition, which isn't quite right from a point of ownership. Also, new Tiles are created here and
    // added to the parent of this Tile. This works, as the Tiles are organised as an object-tree (https://doc.qt.io/qt-5/objecttrees.html)
    // and manage themselfs
    // if a new tile is created, the position is altered accordingly
    Tile* getTileForPosition( QVector3D* position ) {
      // the point is in this Tile -> return it
      if( position->x() >= 0 &&
          position->y() >= 0 &&
          position->x() < sizeOfTile &&
          position->y() < sizeOfTile ) {
        return this;
      } else {
        // calculate relative offset
        qint64 relativeOffsetX = qint64( position->x() / sizeOfTile ) - ( ( position->x() < 0 ) ? 1 : 0 );
        qint64 relativeOffsetY = qint64( position->y() / sizeOfTile ) - ( ( position->y() < 0 ) ? 1 : 0 );
        relativeOffsetX *= qint64( sizeOfTile );
        relativeOffsetY *= qint64( sizeOfTile );

        // offset the position
        if( relativeOffsetX ) {
          position->setX( position->x() - relativeOffsetX );
        }

        if( relativeOffsetY ) {
          position->setY( position->y() - relativeOffsetY );
        }

        // get the tile or create a new one
        return getTileForOffset( this->x + relativeOffsetX, this->y + relativeOffsetY );
      }
    }


    Tile* getTileForOffset( qint64 x, qint64 y ) {
      // find a matching tile and return it
      if( !( parent()->children().empty() ) ) {
        for( auto i : parent()->children() ) {
          Tile* tile = qobject_cast<Tile*>( i );

          if( tile ) {
            if( tile->x == x && tile->y == y ) {
              return tile;
            }
          }
        }
      }

      // no tile found -> create new one
      Tile* tile = new Tile( parent(),
                             x,
                             y,
                             rootEntity, showTileEnabled, color );

      qDebug() << "parent()->children().size():" << parent()->children().size();

      return tile;
    }

  public:
    Qt3DCore::QEntity* tileEntity = nullptr;
    Qt3DCore::QTransform* tileTransform = nullptr;

    Qt3DCore::QEntity* limitsEntity = nullptr;
    Qt3DExtras::QPhongMaterial* material = nullptr;
    QColor color = Qt::red;
    bool showTileEnabled = false;

    Qt3DCore::QEntity* rootEntity = nullptr;
    qint64 x;
    qint64 y;
};

class RootTile: public QObject {
    Q_OBJECT

  public:
    RootTile()
      : QObject() {
    }

    void setShowColor( const QColor& color ) {
      for( auto i : children() ) {
        Tile* tile = qobject_cast<Tile*>( i );
        tile->setShowColor( color );
      }
    }

    void setShowEnable( bool enabled ) {
      for( auto i : children() ) {
        Tile* tile = qobject_cast<Tile*>( i );
        tile->setShowEnable( enabled );
      }
    }

  public:
    bool isLatLonOffsetSet = false;
    double height0 = 0;
    double lon0 = 0;
    double lat0 = 0;
};

#endif // TILE_H
