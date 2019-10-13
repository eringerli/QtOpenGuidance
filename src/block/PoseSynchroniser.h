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

#ifndef POSECACHE_H
#define POSECACHE_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"

class PoseSynchroniser : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseSynchroniser( Tile* tile )
      : BlockBase() {
      rootTile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setTiledPosition( Tile* tile, QVector3D position ) {
      this->position = position;
      emit poseChanged( tile, this->position, orientation, PoseOption::NoOptions );
    }

    void setOrientation( QQuaternion value ) {
      orientation = value;
    }

  signals:
    void poseChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options );

  public:
    virtual void emitConfigSignals() override {
      rootTile = rootTile->getTileForPosition( &position );
      emit poseChanged( rootTile, position, orientation, PoseOption::NoOptions );
    }

  public:
    Tile* rootTile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();
};

class PoseSynchroniserFactory : public BlockFactory {
    Q_OBJECT

  public:
    PoseSynchroniserFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose Synchroniser" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new PoseSynchroniser( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "Tiled Position", SLOT( setTiledPosition( Tile*, QVector3D ) ) );
      b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );

      b->addOutputPort( "Pose", SIGNAL( poseChanged( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // POSECACHE_H

