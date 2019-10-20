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
#include <QSettings>
#include <QStandardPaths>
#include <QEvent>

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
#include <Qt3DRender/QSortPolicy>

#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>

#include "gui/MainWindow.h"
#include "gui/SettingsDialog.h"
#include "gui/GuidanceToolbar.h"
#include "gui/GuidanceToolbarTop.h"
#include "gui/SimulatorToolbar.h"
#include "gui/CameraToolbar.h"
#include "gui/PassToolbar.h"

#include "block/CameraController.h"
#include "block/TractorModel.h"
#include "block/TrailerModel.h"
#include "block/GridModel.h"
#include "block/SectionControlModel.h"
#include "block/XteBarModel.h"
#include "block/MeterBarModel.h"

#include "block/PoseSimulation.h"
#include "block/PoseSynchroniser.h"

#include "kinematic/FixedKinematic.h"
#include "kinematic/TrailerKinematic.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

int main( int argc, char** argv ) {
  QApplication app( argc, argv );
  QApplication::setOrganizationDomain( "QtOpenGuidance.org" );
  QApplication::setApplicationName( "QtOpenGuidance" );

  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
  view->defaultFrameGraph()->setClearColor( QColor( QRgb( 0x4d4d4f ) ) );
  QWidget* container = QWidget::createWindowContainer( view );
  QSize screenSize = view->screen()->size();
  container->setMinimumSize( QSize( 500, 400 ) );
  container->setMaximumSize( screenSize );

  QWidget* widget = new MainWindow;
  widget->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );

  auto* hLayout = new QHBoxLayout( widget );
  auto* vLayout = new QVBoxLayout( widget );
  auto* hLayout2 = new QHBoxLayout( widget );

  auto* guidaceToolbarTop = new GuidanceToolbarTop( widget );
  vLayout->addWidget( guidaceToolbarTop );

  // Camera Toolbar
  auto* cameraToolbar = new CameraToolbar( widget );
  cameraToolbar->setVisible( false );
  hLayout->addWidget( cameraToolbar );

  // passes toolbar
  auto* passesToolbar = new PassToolbar( widget );
  passesToolbar->setVisible( false );
  hLayout2->addWidget( passesToolbar );

  // add the qt3d-widget to the hLayout
  hLayout2->addWidget( container, 1 );
  vLayout->addLayout( hLayout2, 1 );
  hLayout->addLayout( vLayout, 1 );

  auto* input = new Qt3DInput::QInputAspect;
  view->registerAspect( input );

  // Show window
#ifdef ANDROID_ENABLED
  widget->showMaximized();
#else
  widget->show();
  widget->resize( 1200, 800 );
#endif

  // Root entity
  auto* rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

//  cameraEntity->lens()->setPerspectiveProjection( 60.0f, 16.0f / 10.0f, 0.1f, 1000.0f );
  cameraEntity->lens()->setProjectionType( Qt3DRender::QCameraLens::PerspectiveProjection );
  cameraEntity->lens()->setFarPlane( 2000 );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( true ) {
    auto* xaxis = new Qt3DCore::QEntity( rootEntity );
    auto* yaxis = new Qt3DCore::QEntity( rootEntity );
    auto* zaxis = new Qt3DCore::QEntity( rootEntity );

    auto* cylinderMesh = new Qt3DExtras::QCylinderMesh();
    cylinderMesh->setRadius( 0.05f );
    cylinderMesh->setLength( 20.0f );
    cylinderMesh->setRings( 2.0f );
    cylinderMesh->setSlices( 4.0f );

    auto* blueMaterial = new Qt3DExtras::QPhongMaterial();
    blueMaterial->setSpecular( Qt::white );
    blueMaterial->setShininess( 10.0f );
    blueMaterial->setAmbient( Qt::blue );

    auto* redMaterial = new Qt3DExtras::QPhongMaterial();
    redMaterial->setSpecular( Qt::white );
    redMaterial->setShininess( 10.0f );
    redMaterial->setAmbient( Qt::red );

    auto* greenMaterial = new Qt3DExtras::QPhongMaterial();
    greenMaterial->setSpecular( Qt::white );
    greenMaterial->setShininess( 10.0f );
    greenMaterial->setAmbient( Qt::green );

    auto* xTransform = new Qt3DCore::QTransform();
    xTransform->setTranslation( QVector3D( 10, 0, 0 ) );
    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    auto* yTransform = new Qt3DCore::QTransform();
    yTransform->setTranslation( QVector3D( 0, 10, 0 ) );
    auto* zTransform = new Qt3DCore::QTransform();
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

  // sort the object, so transparity works
  Qt3DRender::QFrameGraphNode* framegraph = view->activeFrameGraph();
  auto* sortPolicy = new Qt3DRender::QSortPolicy( rootEntity );
  framegraph->setParent( sortPolicy );
  QVector<Qt3DRender::QSortPolicy::SortType> sortTypes =
    QVector<Qt3DRender::QSortPolicy::SortType>() << Qt3DRender::QSortPolicy::FrontToBack;
  sortPolicy->setSortTypes( sortTypes );
  view->setActiveFrameGraph( framegraph );


  // simulator toolbar
  auto* simulatorToolbar = new SimulatorToolbar( widget );
  simulatorToolbar->setVisible( false );
  vLayout->addWidget( simulatorToolbar );

  // guidance toolbar
  auto* guidaceToolbar = new GuidanceToolbar( widget );
  hLayout->addWidget( guidaceToolbar );

  // Create setting Window
  auto* settingDialog = new SettingsDialog( rootEntity, widget );

  // GUI -> GUI
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    simulatorToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( cameraChanged( bool ) ),
                    cameraToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( passesChanged( bool ) ),
                    passesToolbar, SLOT( setVisible( bool ) ) );
  QObject::connect( guidaceToolbar, SIGNAL( toggleSettings() ),
                    settingDialog, SLOT( toggleVisibility() ) );

  // camera
  BlockFactory* cameraControllerFactory = new CameraControllerFactory( rootEntity, cameraEntity );
  BlockBase* cameraController = cameraControllerFactory->createNewObject();
  cameraControllerFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), cameraController );
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  view->installEventFilter( cameraController );

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

  // grid
  BlockFactory* gridModelFactory = new GridModelFactory( rootEntity, cameraEntity );
  BlockBase* gridModel = gridModelFactory->createNewObject();
  gridModelFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), gridModel );

  QObject::connect( settingDialog, SIGNAL( setGrid( bool ) ),
                    gridModel, SLOT( setGrid( bool ) ) );
  QObject::connect( settingDialog, SIGNAL( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ),
                    gridModel, SLOT( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ) );

  // the processer of Pose etc
  QObject::connect( guidaceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    settingDialog->poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( velocityChanged( float ) ),
                    settingDialog->poseSimulation, SLOT( setVelocity( float ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( frequencyChanged( int ) ),
                    settingDialog->poseSimulation, SLOT( setFrequency( int ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( steerangleChanged( float ) ),
                    settingDialog->poseSimulation, SLOT( setSteerAngle( float ) ) );

  // Guidance Bar
  BlockFactory* xteBarModelFactory = new XteBarModelFactory( widget, guidaceToolbarTop->getCenterLayout() );
  xteBarModelFactory->addToCombobox( settingDialog->getCbNodeType() );
  BlockFactory* meterBarModelFactory = new MeterBarModelFactory( widget, guidaceToolbarTop->getCenterLayout() );
  meterBarModelFactory->addToCombobox( settingDialog->getCbNodeType() );

  QObject::connect( guidaceToolbar, SIGNAL( a_clicked() ),
                    settingDialog->plannerGui, SIGNAL( a_clicked() ) );
  QObject::connect( guidaceToolbar, SIGNAL( b_clicked() ),
                    settingDialog->plannerGui, SIGNAL( b_clicked() ) );
  QObject::connect( guidaceToolbar, SIGNAL( snap_clicked() ),
                    settingDialog->plannerGui, SIGNAL( snap_clicked() ) );
  QObject::connect( guidaceToolbar, SIGNAL( autosteerEnabled( bool ) ),
                    settingDialog->plannerGui, SIGNAL( autosteerEnabled( bool ) ) );
  QObject::connect( guidaceToolbarTop, SIGNAL( turnLeft() ),
                    settingDialog->plannerGui, SIGNAL( turnLeft_clicked() ) );
  QObject::connect( guidaceToolbarTop, SIGNAL( turnRight() ),
                    settingDialog->plannerGui, SIGNAL( turnRight_clicked() ) );

  // Global planner
  QObject::connect( passesToolbar, SIGNAL( passSettingsChanged( int, int, bool, bool ) ),
                    settingDialog->globalPlanner, SLOT( setPassSettings( int, int, bool, bool ) ) );
  QObject::connect( passesToolbar, SIGNAL( passNumberChanged( int ) ),
                    settingDialog->globalPlanner, SLOT( setPassNumber( int ) ) );

  // Section control toolbar
  BlockFactory* sectionControlFactory = new SectionControlModelFactory( widget, vLayout );
  sectionControlFactory->addToCombobox( settingDialog->getCbNodeType() );

  settingDialog->emitAllConfigSignals();


  // load states of checkboxes from global config
  {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    guidaceToolbar->cbSimulatorSetChecked( settings.value( "RunSimulatorOnStart", false ).toBool() );
    guidaceToolbar->cbCameraSetChecked( settings.value( "ShowCameraToolbarOnStart", false ).toBool() );
    guidaceToolbar->cbPassesSetChecked( settings.value( "ShowPassesToolbarOnStart", false ).toBool() );

    if( settings.value( "OpenSettingsDialogOnStart", false ).toBool() ) {
      settingDialog->show();
    }
  }

  // default config
  settingDialog->loadConfigOnStart();
  QObject::connect( widget, SIGNAL( closed() ),
                    settingDialog, SLOT( saveConfigOnExit() ) );

  // camera controller
  QObject::connect( widget, SIGNAL( closed() ),
                    cameraController, SLOT( saveValuesToConfig() ) );

  return QApplication::exec();
}

