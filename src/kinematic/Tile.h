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

class Tile: public QObject {
    Q_OBJECT

  public:
    static constexpr qint64 sizeOfTile = 50;

  public:
    Tile( QObject* parent, qint64 x, qint64 y, Qt3DCore::QEntity* rootEntity )
      : QObject( parent ),
        rootEntity( rootEntity ), x( x ), y( y ) {
      tileTransform = new Qt3DCore::QTransform();
      tileTransform->setTranslation( QVector3D( x, y, 0 ) );

      tileEntity = new Qt3DCore::QEntity( rootEntity );
      tileEntity->addComponent( tileTransform );
    }

    // the check for locality is done here, as it is highly probable, that the point is in the current tile
    // this is an optimition, which isn't quite right from a point of ownership. Also, new Tiles are created here and
    // added to the parent of this Tile. This works, as the Tiles are organised as an object-tree (https://doc.qt.io/qt-5/objecttrees.html)
    // and manage themselfs
    Tile* getTileForPosition( QVector3D position ) {
      // the point is in this Tile -> return it
      if( qint64( position.x() ) >= x &&
          qint64( position.x() ) < ( x + sizeOfTile ) &&
          qint64( position.y() ) >= y &&
          qint64( position.y() ) < ( y + sizeOfTile ) ) {
        return this;
      } else {

        // the point is in an existing Tile -> return it
        if( parent()->children().size() ) {
          for( int i = 0; i < parent()->children().size(); ++i ) {
            Tile* tile = qobject_cast<Tile*>( parent()->children().at( i ) );

            if( tile ) {
              if( ( qint64( position.x() + x ) >= tile->x ) &&
                  ( qint64( position.x() + x ) < ( tile->x + sizeOfTile ) ) &&
                  ( qint64( position.y() + y ) >= tile->y ) &&
                  ( qint64( position.y() + y ) < ( tile->y + sizeOfTile ) ) ) {
                return tile;
              }
            }
          }
        }

        // the point is in no existing Tile -> create new one and return it
        // as the tiles are organised as a tree of QObjects, a new one is automaticaly owned by the parent() of this one
        // https://doc.qt.io/qt-5/objecttrees.html
        Tile* tile = new Tile( parent(),
                               qint64( std::floor( qint64( position.x() + x ) / sizeOfTile ) * sizeOfTile ),
                               qint64( std::floor( qint64( position.y() + y ) / sizeOfTile ) * sizeOfTile ),
                               rootEntity );
        return tile;
      }
    }

  public:
    Qt3DCore::QEntity* tileEntity = nullptr;
    Qt3DCore::QTransform* tileTransform = nullptr;

    Qt3DCore::QEntity* rootEntity;
    qint64 x;
    qint64 y;
};

#endif // TILE_H
