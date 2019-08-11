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

#ifndef POSEFROMPOSITION_H
#define POSEFROMPOSITION_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

class PoseFromPosition : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseFromPosition( Tile* tile )
      : BlockBase() {
      this->tile = tile;
    }

  public slots:
    void setTiledPosition( Tile* tile, QVector3D position ) {

      double y = ( position.y() - this->position.y() ) + ( tile->y - this->tile->y );
      double x = ( position.x() - this->position.x() ) + ( tile->x - this->tile->x );

      if( !( qFuzzyIsNull( x ) && qFuzzyIsNull( y ) ) ) {
        orientation = QQuaternion::fromEulerAngles( QVector3D(
                        0,
                        0,
                        float( qRadiansToDegrees( qAtan2( y, x ) ) )
                      ) );
      }

      this->position = position;
      this->tile = tile;
      emit poseChanged( tile, this->position, orientation );
    }

  signals:
    void poseChanged( Tile* tile, QVector3D position, QQuaternion orientation );

  public:
    virtual void emitConfigSignals() override {
      Tile* rootTile = tile->getTileForOffset( 0, 0 )->getTileForPosition( &position );
      emit poseChanged( rootTile, position, orientation );
    }

  public:
    Tile* tile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();
};

class PoseFromPositionFactory : public BlockFactory {
    Q_OBJECT

  public:
    PoseFromPositionFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose From Position" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new PoseFromPosition( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Tiled Position", SLOT( setTiledPosition( Tile*, QVector3D ) ) );

      b->addOutputPort( "Pose", SIGNAL( poseChanged( Tile*, QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // POSEFROMPOSITION_H

