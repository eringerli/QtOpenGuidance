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
#include <Qt3DExtras/QText2DEntity>

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
        aPointMesh = new Qt3DExtras::QSphereMesh();
        aPointMesh->setRadius( .2f );
        aPointMesh->setSlices( 20 );
        aPointMesh->setRings( 20 );

        aPointTransform = new Qt3DCore::QTransform();

        auto* material = new Qt3DExtras::QPhongMaterial();
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

        auto* material = new Qt3DExtras::QPhongMaterial();
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

      // arrows for passes
      {
        arrowsEntity = new Qt3DCore::QEntity( tile->tileEntity );
        arrowsTransform = new Qt3DCore::QTransform();
        arrowsEntity->addComponent( arrowsTransform );

        activeArrowsEntity = new Qt3DCore::QEntity( arrowsEntity );
        activeArrowsMesh = new LineMesh();
        activeArrowsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
        activeArrowsEntity->addComponent( activeArrowsMesh );
        activeArrowsMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( activeArrowsEntity );
        activeArrowsMaterial->setAlphaBlendingEnabled( true );
        activeArrowsEntity->addComponent( activeArrowsMaterial );

        activeArrowsBackgroundEntity = new Qt3DCore::QEntity( arrowsEntity );
        activeArrowsBackgroundMesh = new LineMesh();
        activeArrowsBackgroundMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
        activeArrowsBackgroundEntity->addComponent( activeArrowsBackgroundMesh );
        activeArrowsBackgroundMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( activeArrowsBackgroundEntity );
        activeArrowsBackgroundEntity->addComponent( activeArrowsBackgroundMaterial );

        otherArrowsEntity = new Qt3DCore::QEntity( arrowsEntity );
        otherArrowsMesh = new LineMesh();
        otherArrowsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
        otherArrowsEntity->addComponent( otherArrowsMesh );
        otherArrowsMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( otherArrowsEntity );
        otherArrowsEntity->addComponent( otherArrowsMaterial );

        otherArrowsBackgroundEntity = new Qt3DCore::QEntity( arrowsEntity );
        otherArrowsBackgroundMesh = new LineMesh();
        otherArrowsBackgroundMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
        otherArrowsBackgroundEntity->addComponent( otherArrowsBackgroundMesh );
        otherArrowsBackgroundMaterial = new Qt3DExtras::QDiffuseSpecularMaterial( otherArrowsBackgroundEntity );
        otherArrowsBackgroundEntity->addComponent( otherArrowsBackgroundMaterial );

        setPassColors();
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


      // arrows/pass
      {
        QVector<QVector3D> arrowPoints;
        float implementWidth = positionLeftEdgeOfImplement.y() - positionRightEdgeOfImplement.y();

        uint16_t arrows = uint16_t( passAreaX / ( arrowSize + distanceBetweenArrows ) );
        QVector3D middlepoint = position - QVector3D( ( passAreaX / 2 ), 0, 0 );

        qDebug() << arrows << middlepoint << passAreaX << arrowSize << distanceBetweenArrows << implementWidth;

        for( uint16_t i = 0; i < arrows; ++i ) {
          QVector3D pointOfArrow = middlepoint + QVector3D( arrowSize, 0, 0 );
          QVector3D backPointOfArrow = middlepoint + QVector3D( arrowSize / 2, 0, 0 );
          QVector3D leftPointOfArrow = middlepoint + QVector3D( 0, implementWidth / 2, 0 );
          QVector3D rightPointOfArrow = middlepoint + QVector3D( 0, -implementWidth / 2, 0 );

          arrowPoints.append( backPointOfArrow );
          arrowPoints.append( pointOfArrow );
          arrowPoints.append( leftPointOfArrow );
          arrowPoints.append( rightPointOfArrow );
          arrowPoints.append( pointOfArrow );
          arrowPoints.append( backPointOfArrow );

          middlepoint += QVector3D( arrowSize + distanceBetweenArrows, 0, 0 );
        }

//        for( auto point : arrowPoints ) {
//          qDebug() << point;
//        }

        activeArrowsMesh->posUpdate( arrowPoints );
      }


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

    void setPassEnabled( bool passEnabled ) {
      this->passEnabled = passEnabled;
      arrowsEntity->setEnabled( passEnabled );
    }

    void setPassSizes( float passAreaX, float passAreaY, float arrowSize, float distanceBetweenArrows ) {
      this->passAreaX = passAreaX;
      this->passAreaY = passAreaY;
      this->arrowSize = arrowSize;
      this->distanceBetweenArrows = distanceBetweenArrows;
    }

    void setPassColors( QColor passActiveArrowColor, QColor passActiveBackgroundColor, QColor passOtherArrowColor, QColor passOtherBackgroundColor ) {
      this->passActiveArrowColor = std::move( passActiveArrowColor );
      this->passActiveBackgroundColor = std::move( passActiveBackgroundColor );
      this->passOtherArrowColor = std::move( passOtherArrowColor );
      this->passOtherBackgroundColor = std::move( passOtherBackgroundColor );
      setPassColors();
    }

  private:
    void setPassColors() {
      activeArrowsMaterial->setAmbient( passActiveArrowColor );
      activeArrowsMaterial->setDiffuse( passActiveArrowColor );
      activeArrowsMaterial->setSpecular( passActiveArrowColor );

      activeArrowsBackgroundMaterial->setAmbient( passActiveBackgroundColor );
      activeArrowsBackgroundMaterial->setDiffuse( passActiveBackgroundColor );
      activeArrowsBackgroundMaterial->setSpecular( passActiveBackgroundColor );

      otherArrowsMaterial->setAmbient( passOtherArrowColor );
      otherArrowsMaterial->setDiffuse( passOtherArrowColor );
      otherArrowsMaterial->setSpecular( passOtherArrowColor );

      otherArrowsBackgroundMaterial->setAmbient( passOtherBackgroundColor );
      otherArrowsBackgroundMaterial->setDiffuse( passOtherBackgroundColor );
      otherArrowsBackgroundMaterial->setSpecular( passOtherBackgroundColor );
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
    Qt3DExtras::QText2DEntity* aTextEntity = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;

    Qt3DExtras::QText2DEntity* bTextEntity = nullptr;
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

    // arrows
    Qt3DCore::QEntity* arrowsEntity = nullptr;
    Qt3DCore::QTransform* arrowsTransform = nullptr;

    Qt3DCore::QEntity* activeArrowsEntity = nullptr;
    LineMesh* activeArrowsMesh = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* activeArrowsMaterial = nullptr;

    Qt3DCore::QEntity* activeArrowsBackgroundEntity = nullptr;
    LineMesh* activeArrowsBackgroundMesh = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* activeArrowsBackgroundMaterial = nullptr;

    Qt3DCore::QEntity* otherArrowsEntity = nullptr;
    LineMesh* otherArrowsMesh = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* otherArrowsMaterial = nullptr;

    Qt3DCore::QEntity* otherArrowsBackgroundEntity = nullptr;
    LineMesh* otherArrowsBackgroundMesh = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* otherArrowsBackgroundMaterial = nullptr;

    // values of the arrows
    bool passEnabled = true;
    float passAreaX = 250, passAreaY = 250, arrowSize = 3, distanceBetweenArrows = 3;
    QColor passActiveArrowColor = QColor( 0xff, 0xff, 0, 200 );
    QColor passActiveBackgroundColor = QColor( 0xf5, 0x9f, 0xbd );
    QColor passOtherArrowColor = QColor( 0x90, 0x90, 0 );
    QColor passOtherBackgroundColor = QColor( 0x9a, 0x64, 0x77 );

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
      auto* b = createBaseBlock( scene, obj );

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

