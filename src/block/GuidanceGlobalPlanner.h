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

#ifndef GLOBALPLANNER_H
#define GLOBALPLANNER_H

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>


class GlobalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlanner( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockBase(),
        rootEntity( rootEntity ) {
      this->tile = tile->getTileForOffset( 0, 0 );
      this->tile00 = tile->getTileForOffset( 0, 0 );

      // a point marker -> orange
      {
        aPointEntity = new Qt3DCore::QEntity( tile->tileEntity );

        aPointMesh = new Qt3DExtras::QSphereMesh();
        aPointMesh->setRadius( .2f );
        aPointMesh->setSlices( 20 );
        aPointMesh->setRings( 20 );

        aPointTransform = new Qt3DCore::QTransform();

        Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse( QColor( "orange" ) );

        aPointEntity->addComponent( aPointMesh );
        aPointEntity->addComponent( material );
        aPointEntity->addComponent( aPointTransform );
        aPointEntity->setEnabled( false );

        aTextEntity = new Qt3DCore::QEntity( aPointEntity );
        Qt3DExtras::QExtrudedTextMesh* aTextMesh = new Qt3DExtras::QExtrudedTextMesh();
        aTextMesh->setText( "A" );
        aTextMesh->setDepth( 0.05f );

        aTextEntity->setEnabled( true );
        aTextTransform = new Qt3DCore::QTransform();
        aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        aTextTransform->setScale( 2.0f );
        aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
        aTextEntity->addComponent( aTextTransform );
        aTextEntity->addComponent( aTextMesh );
        aTextEntity->addComponent( material );
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

        bTextEntity = new Qt3DCore::QEntity( bPointEntity );
        Qt3DExtras::QExtrudedTextMesh* bTextMesh = new Qt3DExtras::QExtrudedTextMesh();
        bTextMesh->setText( "B" );
        bTextMesh->setDepth( 0.05f );

        bTextEntity->setEnabled( true );
        bTextTransform = new Qt3DCore::QTransform();
        bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
        bTextTransform->setScale( 2.0f );
        bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
        bTextEntity->addComponent( bTextTransform );
        bTextEntity->addComponent( bTextMesh );
        bTextEntity->addComponent( material );
      }

      // line marker
      {
        pathEntity = new Qt3DCore::QEntity( tile00->tileEntity );
        pathEntity->setEnabled( false );

        pathTransform = new Qt3DCore::QTransform();
        pathEntity->addComponent( pathTransform );

        pathMesh = new LineMesh();
        pathEntity->addComponent( pathMesh );

        pathMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( pathEntity );
        pathMaterial->setAmbient( Qt::red );
        pathEntity->addComponent( pathMaterial );
      }
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->tile = tile;
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );
      }
    }

    void setPoseLeftEdge( Tile*, QVector3D position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutTiling ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;
      }
    }

    void setPoseRightEdge( Tile*, QVector3D position, QQuaternion, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutTiling ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;
      }
    }

    void a_clicked() {
      aPointEntity->setParent( tile->tileEntity );
      aPointTransform->setTranslation( position );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );
      pathEntity->setEnabled( false );

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

      headingOfABLine = atan2( y1 - y2, x1 - x2 ) - M_PI;

      QVector<QVector3D> linePoints;

      // extend the points 200m in either direction

      double ab = qSqrt( qPow( ( x2 - x1 ), 2 ) + qPow( ( y2 - y1 ), 2 ) );
      double ac = -200;
      linePoints.append( QVector3D( x1 + ( ac * ( x2 - x1 ) / ab ), y1 + ( ac * ( y2 - y1 ) / ab ), position.z() ) );
      ac = 200;
      linePoints.append( QVector3D( x2 + ( ac * ( x2 - x1 ) / ab ), y2 + ( ac * ( y2 - y1 ) / ab ), position.z() ) );
      pathMesh->posUpdate( linePoints );

      pathEntity->setEnabled( true );

      QVector<QSharedPointer<PathPrimitive>> plan;
//      ac = -200;
//      double x1tmp = x1 + (ac * (x2 - x1) / ab);
//      double y1tmp = y1 + (ac * (y2 - y1) / ab);
//      ac = 200;
//      double x2tmp = x1 + (ac * (x2 - x1) / ab);
//      double y2tmp = y1 + (ac * (y2 - y1) / ab);

      plan.append( QSharedPointer<PathPrimitive>( new PathPrimitiveLine( x1, y1, x2, y2, false, false ) ) );
      emit planChanged( plan );

      qDebug() << "b_clicked()" << x1 << y1 << x2 << y2 << x1 - x2 << y1 - y2 << qRadiansToDegrees( headingOfABLine );
    }

    void snap_clicked() {
      qDebug() << "snap_clicked()";

      QVector<QSharedPointer<PathPrimitive>> plan;
//      ac = -200;
//      double x1tmp = x1 + (ac * (x2 - x1) / ab);
//      double y1tmp = y1 + (ac * (y2 - y1) / ab);
//      ac = 200;
//      double x2tmp = x1 + (ac * (x2 - x1) / ab);
//      double y2tmp = y1 + (ac * (y2 - y1) / ab);

      plan.append( QSharedPointer<PathPrimitive>( new PathPrimitiveLine( position.x(), position.y(), position.x(), position.y() + 20, true, false ) ) );
      plan.append( QSharedPointer<PathPrimitive>( new PathPrimitiveLine( position.x() + 20, position.y(), position.x() + 20, position.y() + 20, false, false ) ) );

      emit planChanged( plan );
    }

    void turnLeft_clicked() {
      qDebug() << "turnLeft_clicked()";
    }
    void turnRight_clicked() {
      qDebug() << "turnRight_clicked()";
    }

  signals:
    void planChanged( QVector<QSharedPointer<PathPrimitive>> );

  public:
    Tile* tile = nullptr;
    Tile* tile00 = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();

    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    double headingOfABLine = 0;

    QVector3D positionLeftEdgeOfImplement = QVector3D();
    QVector3D positionRightEdgeOfImplement = QVector3D();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;

    // text
    Qt3DCore::QEntity* aTextEntity = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;

    Qt3DCore::QEntity* bTextEntity = nullptr;
    Qt3DCore::QTransform* bTextTransform = nullptr;

    // markers
    Qt3DCore::QEntity* aPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* aPointMesh = nullptr;
    Qt3DCore::QTransform* aPointTransform = nullptr;

    Qt3DCore::QEntity* bPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* bPointMesh = nullptr;
    Qt3DCore::QTransform* bPointTransform = nullptr;

    // path
    Qt3DCore::QEntity* pathEntity = nullptr;
    LineMesh* pathMesh = nullptr;
    Qt3DCore::QTransform* pathTransform = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* pathMaterial = nullptr;
};

class GlobalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerFactory( Tile* tile, Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        tile( tile ),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new GlobalPlanner( tile, rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Left Edge", SLOT( setPoseLeftEdge( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "Pose Right Edge", SLOT( setPoseRightEdge( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );
      b->addInputPort( "A clicked", SLOT( a_clicked() ) );
      b->addInputPort( "B clicked", SLOT( b_clicked() ) );
      b->addInputPort( "Snap clicked", SLOT( snap_clicked() ) );
      b->addInputPort( "Turn Left", SLOT( turnLeft_clicked() ) );
      b->addInputPort( "Turn Right", SLOT( turnRight_clicked() ) );

      b->addOutputPort( "Plan", SIGNAL( planChanged( QVector<QSharedPointer<PathPrimitive>> ) ) );

      return b;
    }

  private:
    Tile* tile = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // GLOBALPLANNER_H

