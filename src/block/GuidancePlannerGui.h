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

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QText2DEntity>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"

class PlannerGui : public BlockBase {
    Q_OBJECT

  public:
    explicit PlannerGui( Tile* tile )
      : BlockBase() {
      this->tile = tile->getTileForOffset( 0, 0 );

      // a point marker -> orange
      {
        aPointMesh = new Qt3DExtras::QSphereMesh();
        aPointMesh->setRadius( .2f );
        aPointMesh->setSlices( 20 );
        aPointMesh->setRings( 20 );

        aPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "orange" ) );

        aPointEntity = new Qt3DCore::QEntity( tile->tileEntity );
        aPointEntity->addComponent( aPointMesh );
        aPointEntity->addComponent( material );
        aPointEntity->addComponent( aPointTransform );
        aPointEntity->setEnabled( false );

        aTextEntity = new Qt3DExtras::QText2DEntity( aPointEntity );
        aTextEntity->setText( "A" );
        aTextEntity->setHeight( 10 );
        aTextEntity->setWidth( 10 );
        aTextEntity->setColor( Qt::green );
        aTextEntity->setFont( QFont( "Arial Narrow", 10 ) );
        aTextTransform = new Qt3DCore::QTransform();
        aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        aTextTransform->setScale( 0.2f );
        aTextEntity->addComponent( aTextTransform );
      }

      // b point marker -> purple
      {
        bPointMesh = new Qt3DExtras::QSphereMesh();
        bPointMesh->setRadius( .2f );
        bPointMesh->setSlices( 20 );
        bPointMesh->setRings( 20 );

        bPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "purple" ) );

        bPointEntity = new Qt3DCore::QEntity( tile->tileEntity );
        bPointEntity->addComponent( bPointMesh );
        bPointEntity->addComponent( material );
        bPointEntity->addComponent( bPointTransform );
        bPointEntity->setEnabled( false );

        bTextEntity = new Qt3DExtras::QText2DEntity( bPointEntity );
        bTextEntity->setText( "B" );
        bTextEntity->setHeight( 10 );
        bTextEntity->setWidth( 10 );
        bTextEntity->setColor( Qt::green );
        bTextEntity->setFont( QFont( "Arial Narrow", 10 ) );
        bTextTransform = new Qt3DCore::QTransform();
        bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        bTextTransform->setScale( 0.2f );
        bTextEntity->addComponent( bTextTransform );
      }
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      this->tile = tile;
      this->position = position;
      this->orientation = orientation;

      aPointTransform->setRotation( orientation );
      bPointTransform->setRotation( orientation );
    }

    void a_clicked() {
      aPointEntity->setParent( tile->tileEntity );
      aPointTransform->setTranslation( position );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );

      x1 = double( position.x() ) + tile->x;
      y1 = double( position.y() ) + tile->y;

      qDebug() << "a_clicked()" << x1 << y1;
    }

    void b_clicked() {
      bPointEntity->setParent( tile->tileEntity );
      bPointTransform->setTranslation( position );
      bPointEntity->setEnabled( true );

      x2 = double( position.x() ) + tile->x;
      y2 = double( position.y() ) + tile->y;

      headingOfABLine = atan2( x1 - x2, y1 - y2 ) + M_PI_2;

      qDebug() << "b_clicked()" << x1 << y1 << x2 << y2 << x1 - x2 << y1 - y2 << qRadiansToDegrees( headingOfABLine );
    }

    void snap_clicked() {
      qDebug() << "snap_clicked()";
    }

  signals:


  public:
    Tile* tile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();

    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    double headingOfABLine = 0;

  private:
    Qt3DExtras::QText2DEntity* aTextEntity = nullptr;
    Qt3DExtras::QText2DEntity* bTextEntity = nullptr;

    Qt3DCore::QEntity* aPointEntity = nullptr;
    Qt3DCore::QEntity* bPointEntity = nullptr;

    Qt3DExtras::QSphereMesh* aPointMesh = nullptr;
    Qt3DExtras::QSphereMesh* bPointMesh = nullptr;

    Qt3DCore::QTransform* aPointTransform = nullptr;
    Qt3DCore::QTransform* bPointTransform = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;
    Qt3DCore::QTransform* bTextTransform = nullptr;

};

class PlannerGuiFactory : public BlockFactory {
    Q_OBJECT

  public:
    PlannerGuiFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Planner GUI" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
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

