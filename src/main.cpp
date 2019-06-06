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

// tractor-trailer kinematics
// https://userpages.uni-koblenz.de/~zoebel/pdf/Mamotep2.pdf
// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

// axis/rotation conventions
// https://ch.mathworks.com/help/driving/ug/coordinate-systems.html

#include <QGuiApplication>

#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCameraLens>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QSlider>
#include <QtGui/QScreen>

#include <Qt3DInput/QInputAspect>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QPointLight>

#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>

#include "gui/SettingsDialog.h"
#include "gui/GuidanceToolbar.h"
#include "gui/SimulatorToolbar.h"
#include "gui/CameraToolbar.h"

#include "block/CameraController.h"
#include "block/TractorModel.h"
#include "block/TrailerModel.h"
#include "block/GridModel.h"

#include "block/PoseSimulation.h"
#include "block/PoseSynchroniser.h"


#include "kinematic/FixedKinematic.h"
#include "kinematic/TrailerKinematic.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

int main( int argc, char** argv ) {
  QApplication app( argc, argv );

  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
  view->defaultFrameGraph()->setClearColor( QColor( QRgb( 0x4d4d4f ) ) );
  QWidget* container = QWidget::createWindowContainer( view );
  QSize screenSize = view->screen()->size();
  container->setMinimumSize( QSize( 500, 400 ) );
  container->setMaximumSize( screenSize );

  QWidget* widget = new QWidget;
  QHBoxLayout* hLayout = new QHBoxLayout( widget );

  // Camera Toolbar
  CameraToolbar* cameraToolbar = new CameraToolbar( widget );
  cameraToolbar->setVisible( false );
  hLayout->addWidget( cameraToolbar );

  // add the qt3d-widget to the hLayout
  hLayout->addWidget( container, 1 );



  widget->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );

  Qt3DInput::QInputAspect* input = new Qt3DInput::QInputAspect;
  view->registerAspect( input );

  // Root entity
  Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

//  cameraEntity->lens()->setPerspectiveProjection( 60.0f, 16.0f / 10.0f, 0.1f, 1000.0f );
  cameraEntity->lens()->setProjectionType( Qt3DRender::QCameraLens::PerspectiveProjection );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity( rootEntity );
  Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight( lightEntity );
  light->setColor( "white" );
  light->setIntensity( 1.0f );
  lightEntity->addComponent( light );
  Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform( lightEntity );
  lightTransform->setTranslation( cameraEntity->position() );
  lightEntity->addComponent( lightTransform );

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( 1 ) {
    Qt3DCore::QEntity* xaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* yaxis = new Qt3DCore::QEntity( rootEntity );
    Qt3DCore::QEntity* zaxis = new Qt3DCore::QEntity( rootEntity );

    Qt3DExtras::QCylinderMesh* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.05f );
    cylinderMesh->setLength( 20.0f );
    cylinderMesh->setRings( 2.0f );
    cylinderMesh->setSlices( 4.0f );

    Qt3DExtras::QPhongMaterial* blueMaterial = new Qt3DExtras::QPhongMaterial();
    blueMaterial->setSpecular( Qt::white );
    blueMaterial->setShininess( 10.0f );
    blueMaterial->setAmbient( Qt::blue );

    Qt3DExtras::QPhongMaterial* redMaterial = new Qt3DExtras::QPhongMaterial();
    redMaterial->setSpecular( Qt::white );
    redMaterial->setShininess( 10.0f );
    redMaterial->setAmbient( Qt::red );

    Qt3DExtras::QPhongMaterial* greenMaterial = new Qt3DExtras::QPhongMaterial();
    greenMaterial->setSpecular( Qt::white );
    greenMaterial->setShininess( 10.0f );
    greenMaterial->setAmbient( Qt::green );

    Qt3DCore::QTransform* xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation( QVector3D( 10, 0, 0 ) );
    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    Qt3DCore::QTransform* yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation( QVector3D( 0, 10, 0 ) );
    Qt3DCore::QTransform* zTransform = new Qt3DCore::QTransform();
    zTransform->setTranslation( QVector3D( 0, 0, 10 ) );
    zTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 1, 0, 0 ), 90 ) );


    xaxis->addComponent( cylinderMesh );
    xaxis->addComponent( redMaterial );
    xaxis->addComponent( xTransform );
    yaxis->addComponent( cylinderMesh );
    yaxis->addComponent( greenMaterial );
    yaxis->addComponent( yTransform );
    zaxis->addComponent( cylinderMesh );
    zaxis->addComponent( blueMaterial );
    zaxis->addComponent( zTransform );
  }

  // Set root object of the scene
  view->setRootEntity( rootEntity );

  SimulatorToolbar* simulatorToolbar = new SimulatorToolbar( widget );
  simulatorToolbar->setVisible( false );
  hLayout->addWidget( simulatorToolbar );

  GuidanceToolbar* guidaceToolbar = new GuidanceToolbar( widget );
  hLayout->addWidget( guidaceToolbar );

  // Create setting Window
  SettingsDialog* settingDialog = new SettingsDialog( rootEntity, widget );

  // GUI -> GUI
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    simulatorToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( cameraChanged( bool ) ),
                    cameraToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( toggleSettings() ),
                    settingDialog, SLOT( toggleVisibility() ) );



  // IMPORTANT: the order of the systemblocks should not change, as they get their id in order of creation
  // DON'T BREAK SAVED CONFIGS!

  // camera
  GuidanceFactory* cameraControllerFactory = new CameraControllerFactory( rootEntity, cameraEntity );
  GuidanceBase* cameraController = cameraControllerFactory->createNewObject();
  cameraControllerFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), cameraController );

  QObject::connect( cameraToolbar, SIGNAL( zoomIn() ),
                    cameraController, SLOT( zoomIn() ) );
  QObject::connect( cameraToolbar, SIGNAL( zoomOut() ),
                    cameraController, SLOT( zoomOut() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltUp() ),
                    cameraController, SLOT( tiltUp() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltDown() ),
                    cameraController, SLOT( tiltDown() ) );
  QObject::connect( cameraToolbar, SIGNAL( panLeft() ),
                    cameraController, SLOT( panLeft() ) );
  QObject::connect( cameraToolbar, SIGNAL( panRight() ),
                    cameraController, SLOT( panRight() ) );
  QObject::connect( cameraToolbar, SIGNAL( resetCamera() ),
                    cameraController, SLOT( resetCamera() ) );
  QObject::connect( cameraToolbar, SIGNAL( setMode( int ) ),
                    cameraController, SLOT( setMode( int ) ) );



  // the processer of Pose etc
  GuidanceFactory* poseSimulationFactory = new PoseSimulationFactory();
  GuidanceBase* poseSimulation = poseSimulationFactory->createNewObject();
  poseSimulationFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), poseSimulation );

  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( velocityChanged( float ) ),
                    poseSimulation, SLOT( setVelocity( float ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( frequencyChanged( int ) ),
                    poseSimulation, SLOT( setFrequency( int ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( steerangleChanged( float ) ),
                    poseSimulation, SLOT( setSteerAngle( float ) ) );

  // grid
  GuidanceFactory* gridModelFactory = new GridModelFactory( rootEntity );
  GuidanceBase* gridModel = gridModelFactory->createNewObject();
  gridModelFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), gridModel );
  QObject::connect( settingDialog, SIGNAL( setGrid( bool ) ),
                    gridModel, SLOT( setGrid( bool ) ) );
  QObject::connect( settingDialog, SIGNAL( setGridValues( float, float, float, QColor ) ),
                    gridModel, SLOT( setGridValues( float, float, float, QColor ) ) );

  // make the light follow the camera
  QObject::connect( cameraEntity, SIGNAL( positionChanged( QVector3D ) ),
                    lightTransform, SLOT( setTranslation( QVector3D ) ) );

  settingDialog->emitAllConfigSignals();

  // Show window
  widget->show();
  widget->resize( 1200, 800 );

  return app.exec();
}

