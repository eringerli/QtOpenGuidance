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

#ifndef PLANNERGUI_H
#define PLANNERGUI_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "GuidanceBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

class PlannerGui : public GuidanceBase {
    Q_OBJECT

  public:
    explicit PlannerGui( Tile* tile )
      : GuidanceBase() {
      rootTile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      this->position = position;
      this->orientation = orientation;
    }

  signals:

  public:
    virtual void emitConfigSignals() override {
      rootTile = rootTile->getTileForPosition( &position );
    }

  public:
    Tile* rootTile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();
};

class PlannerGuiFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    PlannerGuiFactory( Tile* tile )
      : GuidanceFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Planner GUI" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new PlannerGui( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // PLANNERGUI_H

