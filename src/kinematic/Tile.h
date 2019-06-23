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

#include <QDebug>
#include "../3d/drawline.h"

class RootTile: public QObject {
    Q_OBJECT

  public:
    RootTile()
      : QObject() {
    }

  public:
    bool isLatLonOffsetSet = false;
    double height0 = 0;
    double lon0 = 0;
    double lat0 = 0;
};

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
    Tile( QObject* parent, qint64 x, qint64 y, Qt3DCore::QEntity* rootEntity )
      : QObject( parent ),
        rootEntity( rootEntity ), x( x ), y( y ) {
      tileTransform = new Qt3DCore::QTransform();
      tileTransform->setTranslation( QVector3D( x, y, 0 ) );

      qDebug() << "new Tile:" << x << y << this;

      tileEntity = new Qt3DCore::QEntity( rootEntity );
      tileEntity->addComponent( tileTransform );

      // comment in to draw lines around tile
//      QVector3D pt1( 0, 0, 0 );
//      QVector3D pt2( 50, 0, 0 );
//      drawLine( pt1, pt2,  Qt::red, tileEntity );
//      pt1 = QVector3D( 50, 50, 0 );
//      drawLine( pt1, pt2,  Qt::red, tileEntity );
//      pt2 = QVector3D( 0, 50, 0 );
//      drawLine( pt1, pt2,  Qt::red, tileEntity );
//      pt1 = QVector3D( 0, 0, 0 );
//      drawLine( pt1, pt2,  Qt::red, tileEntity );

    }

    // the check for locality is done here, as it is highly probable, that the point is in the current tile
    // this is an optimition, which isn't quite right from a point of ownership. Also, new Tiles are created here and
    // added to the parent of this Tile. This works, as the Tiles are organised as an object-tree (https://doc.qt.io/qt-5/objecttrees.html)
    // and manage themselfs
    // if a new tile is created, the position is altered accordingly
    Tile* getTileForPosition( double& x, double& y ) {
      // the point is in this Tile -> return it
      if( x >= 0 &&
          x >= 0 &&
          y < sizeOfTileDouble &&
          y < sizeOfTileDouble ) {
        return this;
      } else {
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
      if( parent()->children().size() ) {
        for( int i = 0; i < parent()->children().size(); ++i ) {
          Tile* tile = qobject_cast<Tile*>( parent()->children().at( i ) );

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
                             rootEntity );

      return tile;
    }

  public:
    Qt3DCore::QEntity* tileEntity = nullptr;
    Qt3DCore::QTransform* tileTransform = nullptr;

    Qt3DCore::QEntity* rootEntity;
    qint64 x;
    qint64 y;
};

#endif // TILE_H
