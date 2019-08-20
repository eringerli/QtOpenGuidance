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

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

class PlannerGui : public BlockBase {
    Q_OBJECT

  public:
    explicit PlannerGui( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockBase(),
        rootEntity( rootEntity ) {
      this->tile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      this->tile = tile;
      this->position = position;
      this->orientation = orientation;
    }

  signals:
    void a_clicked();
    void b_clicked();
    void snap_clicked();
    void autosteerEnabled( bool );
    void turnLeft_clicked();
    void turnRight_clicked();
    void xteChanged( double );

  public:
    Tile* tile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

class PlannerGuiFactory : public BlockFactory {
    Q_OBJECT

  public:
    PlannerGuiFactory( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        tile( tile ),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Planner GUI" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new PlannerGui( tile, rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );
      b->addOutputPort( "A clicked", SIGNAL( a_clicked() ) );
      b->addOutputPort( "B clicked", SIGNAL( b_clicked() ) );
      b->addOutputPort( "Snap clicked", SIGNAL( snap_clicked() ) );
      b->addOutputPort( "Turn Left", SIGNAL( turnLeft_clicked() ) );
      b->addOutputPort( "Turn Right", SIGNAL( turnRight_clicked() ) );
      b->addOutputPort( "Autosteer Enabled", SIGNAL( autosteerEnabled( bool ) ) );
      b->addInputPort( "XTE", SIGNAL( xteChanged( double ) ) );

      return b;
    }

  private:
    Tile* tile;
    Qt3DCore::QEntity* rootEntity;
};

#endif // PLANNERGUI_H

