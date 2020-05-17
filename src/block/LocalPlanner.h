// Copyright( C ) 2020 Christian Riggenbach
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

#pragma once

#include <QObject>

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"
#include "../kinematic/Plan.h"

#include "../gui/GuidanceTurning.h"

#include "../gui/MyMainWindow.h"
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>


#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include "../3d/BufferMesh.h"

class LocalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit LocalPlanner( const QString& uniqueName,
                           MyMainWindow* mainWindow,
                           Qt3DCore::QEntity* rootEntity
                         )
      : BlockBase(),
        rootEntity( rootEntity ) {
      widget = new GuidanceTurning( mainWindow );
      dock = new KDDockWidgets::DockWidget( uniqueName );

      QObject::connect( widget, &GuidanceTurning::turnLeftToggled, this, &LocalPlanner::turnLeftToggled );
      QObject::connect( widget, &GuidanceTurning::turnRightToggled, this, &LocalPlanner::turnRightToggled );
      QObject::connect( widget, &GuidanceTurning::numSkipChanged, this, &LocalPlanner::numSkipChanged );
      QObject::connect( this, &LocalPlanner::resetTurningStateOfDock, widget, &GuidanceTurning::resetTurningState );

      {
        m_baseEntity = new Qt3DCore::QEntity( rootEntity );
        m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
        m_baseEntity->addComponent( m_baseTransform );

        m_segmentsEntity = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
        m_segmentsEntity->setEnabled( false );
        m_segmentsEntity2->setEnabled( false );
        m_segmentsEntity3->setEnabled( false );

        m_segmentsMesh = new BufferMesh( m_segmentsEntity );
        m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
        m_segmentsEntity->addComponent( m_segmentsMesh );

        m_segmentsMesh2 = new BufferMesh( m_segmentsEntity2 );
        m_segmentsMesh2->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
        m_segmentsEntity2->addComponent( m_segmentsMesh2 );

        m_segmentsMesh3 = new BufferMesh( m_segmentsEntity3 );
        m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
        m_segmentsEntity3->addComponent( m_segmentsMesh3 );

        m_segmentsMesh4 = new BufferMesh( m_segmentsEntity4 );
        m_segmentsMesh4->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
        m_segmentsEntity4->addComponent( m_segmentsMesh4 );

        m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
        m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
        m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
        m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

        m_segmentsMaterial->setAmbient( Qt::green );
        m_segmentsMaterial2->setAmbient( Qt::yellow );
        m_segmentsMaterial3->setAmbient( Qt::blue );
        m_segmentsMaterial4->setAmbient( Qt::red );

//        m_segmentsMaterial->setAmbient( Qt::darkGreen );
//        m_segmentsMaterial2->setAmbient( Qt::darkYellow );
//        m_segmentsMaterial3->setAmbient( Qt::darkBlue );
//        m_segmentsMaterial4->setAmbient( Qt::darkRed );

        m_segmentsEntity->addComponent( m_segmentsMaterial );
        m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
        m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
        m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
      }
    }

    ~LocalPlanner() {
      dock->deleteLater();
      widget->deleteLater();
    }

  public slots:
    void setName( const QString& name ) override {
      dock->setTitle( name );
      dock->toggleAction()->setText( QStringLiteral( "Turning Dock: " ) + name );
    }

    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options );

    void setPlan( const Plan& plan );

    void setSteeringAngle( double steeringAngle ) {
      this->steeringAngleDegrees = steeringAngle;
    }

    void setPathHysteresis( const double pathHysteresis ) {
      this->pathHysteresis = pathHysteresis;
    }

    void setMinRadius( const double minRadius ) {
      this->minRadius = minRadius;
    }

    void turnLeftToggled( bool state );
    void turnRightToggled( bool state );
    void numSkipChanged( int left, int right );

  signals:
    void planChanged( const Plan& );
    void triggerPlanPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options );
    void resetTurningStateOfDock();

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();
    double steeringAngleDegrees = 0;
    double pathHysteresis = 0.5;
    double minRadius = 10;

    GuidanceTurning* widget = nullptr;
    KDDockWidgets::DockWidget* dock = nullptr;

  private:
    void calculateTurning( bool changeExistingTurn );
    void showPlan();

    Plan globalPlan;
    Plan plan;

    Plan::PrimitiveSharedPointer lastPrimitive = nullptr;

    bool turningLeft = false;
    bool turningRight = false;
    int leftSkip = 1;
    int rightSkip = 1;
    Point_2 positionTurnStart = Point_2( 0, 0 );
    double headingTurnStart = 0;
    Plan::ConstPrimitiveIterator lastSegmentOfTurn;
    Plan::ConstPrimitiveIterator targetSegmentOfTurn;


    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    Qt3DCore::QEntity* m_segmentsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity2 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity3 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity4 = nullptr;
    BufferMesh* m_pointsMesh = nullptr;
    BufferMesh* m_segmentsMesh = nullptr;
    BufferMesh* m_segmentsMesh2 = nullptr;
    BufferMesh* m_segmentsMesh3 = nullptr;
    BufferMesh* m_segmentsMesh4 = nullptr;
    Qt3DExtras::QPhongMaterial* m_pointsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial2 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial3 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial4 = nullptr;
};

class LocalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    LocalPlannerFactory( MyMainWindow* mainWindow,
                         KDDockWidgets::Location location,
                         QMenu* menu,
                         Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Local Planner" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* object = new LocalPlanner( getNameOfFactory() + QString::number( id ),
                                       mainWindow,
                                       rootEntity );
      auto* b = createBaseBlock( scene, object, id );

      object->dock->setTitle( getNameOfFactory() );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      mainWindow->addDockWidget( object->dock, location );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Path Hysteresis" ), QLatin1String( SLOT( setPathHysteresis( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Minimum Radius" ), QLatin1String( SLOT( setMinRadius( const double ) ) ) );

      b->addOutputPort( QStringLiteral( "Trigger Plan Pose" ), QLatin1String( SIGNAL( triggerPlanPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
};
