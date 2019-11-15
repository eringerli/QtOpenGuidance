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
#include "gui/GuidanceTurning.h"
#include "gui/SimulatorToolbar.h"
#include "gui/CameraToolbar.h"
#include "gui/PassToolbar.h"

#include "block/CameraController.h"
#include "block/FpsMeasurement.h"
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

#include "aspect/fpsaspect.h"
#include "aspect/fpsmonitor.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

#if defined (Q_OS_ANDROID)
#include <QtAndroid>
const QVector<QString> permissions( {"android.permission.INTERNET",
                                     "android.permission.WRITE_EXTERNAL_STORAGE",
                                     "android.permission.READ_EXTERNAL_STORAGE"
                                    } );
#endif

int main( int argc, char** argv ) {
//  // hack to make the app apear without cropped qt3d-widget
  QCoreApplication::setAttribute( Qt::AA_DisableHighDpiScaling );
//    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

//  qputenv( "QT_AUTO_SCREEN_SCALE_FACTOR", "1.5" );
//  qputenv( "QT_SCALE_FACTOR_ROUNDING_POLICY","PassThrough");
//  qputenv( "QT_SCALE_FACTOR", "1" );

//QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );
//  QCoreApplication::setAttribute( Qt::AA_Use96Dpi );

  // make qDebug() more expressive
  qSetMessagePattern( "%{file}:%{line}, %{function}: %{message}" );

  QApplication app( argc, argv );
  QApplication::setOrganizationDomain( "QtOpenGuidance.org" );
  QApplication::setApplicationName( "QtOpenGuidance" );


  //Request required permissions at runtime on android
#ifdef Q_OS_ANDROID

  for( const QString& permission : permissions ) {
    auto result = QtAndroid::checkPermission( permission );

    if( result == QtAndroid::PermissionResult::Denied ) {
      auto resultHash = QtAndroid::requestPermissionsSync( QStringList( {permission} ) );

      if( resultHash[permission] == QtAndroid::PermissionResult::Denied )
        return 0;
    }
  }

#endif

  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();

  view->defaultFrameGraph()->setClearColor( QColor( QRgb( 0x4d4d4f ) ) );

  QWidget* container = QWidget::createWindowContainer( view );
//  QSize screenSize = view->screen()->size();
//  container->setMinimumSize( QSize( 500, 400 ) );
//  container->setMaximumSize( screenSize );
  container->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  // create MainWindow and set the parameters for the docks
  MainWindow* mainWindow = new MainWindow;
  QWidget* widget = new QWidget( mainWindow );
  mainWindow->setCentralWidget( container );
  mainWindow->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );
  mainWindow->setDockOptions( QMainWindow::AnimatedDocks |
                              QMainWindow::AllowNestedDocks |
                              QMainWindow::AllowTabbedDocks |
                              QMainWindow::VerticalTabs );
  mainWindow->setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
  mainWindow->setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
  mainWindow->setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

  // Root entity for Qt3D
  auto* rootEntity = new Qt3DCore::QEntity();

  // Create setting Window
  auto* settingDialog = new SettingsDialog( rootEntity, mainWindow, widget );

  // FPS measuring: register the aspect
  view->registerAspect( new FpsAspect );

//  auto* input = new Qt3DInput::QInputAspect;
//  view->registerAspect( input );

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

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
  auto* sortPolicy = new Qt3DRender::QSortPolicy( );
  framegraph->setParent( sortPolicy );
  QVector<Qt3DRender::QSortPolicy::SortType> sortTypes =
    QVector<Qt3DRender::QSortPolicy::SortType>() << Qt3DRender::QSortPolicy::FrontToBack;
  sortPolicy->setSortTypes( sortTypes );
  view->setActiveFrameGraph( sortPolicy );


  // guidance toolbar
  auto* guidanceToolbar = new GuidanceToolbar( widget );
  QDockWidget* guidaceToolbarDock = new QDockWidget( mainWindow );
  guidaceToolbarDock->setWidget( guidanceToolbar );
  guidaceToolbarDock->setWindowTitle( guidanceToolbar->windowTitle() );
  guidaceToolbarDock->setObjectName( QStringLiteral( "GuidanceToolbar" ) );
  guidaceToolbarDock->setTitleBarWidget( new QWidget( guidaceToolbarDock ) );
  guidaceToolbarDock->setFeatures( QDockWidget::NoDockWidgetFeatures );
  mainWindow->addDockWidget( Qt::RightDockWidgetArea, guidaceToolbarDock );

  // turning Toolbar
  auto* turningToolbar = new GuidanceTurning( mainWindow );
  QDockWidget* turningToolbarDock = new QDockWidget( mainWindow );
  turningToolbarDock->setWidget( turningToolbar );
  turningToolbarDock->setWindowTitle( turningToolbar->windowTitle() );
  turningToolbarDock->setObjectName( QStringLiteral( "TurningToolbar" ) );
  turningToolbarDock->setFeatures( QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar );
  mainWindow->addDockWidget( Qt::TopDockWidgetArea, turningToolbarDock );
  guidanceToolbar->menu->addAction( turningToolbarDock->toggleViewAction() );

  // camera Toolbar
  auto* cameraToolbar = new CameraToolbar( widget );
  cameraToolbar->setVisible( false );
  QDockWidget* cameraToolbarDock = new QDockWidget( mainWindow );
  cameraToolbarDock->setWidget( cameraToolbar );
  cameraToolbarDock->setWindowTitle( cameraToolbar->windowTitle() );
  cameraToolbarDock->setObjectName( QStringLiteral( "CameraToolbar" ) );
  mainWindow->addDockWidget( Qt::LeftDockWidgetArea, cameraToolbarDock );
  guidanceToolbar->menu->addAction( cameraToolbarDock->toggleViewAction() );

  // passes toolbar
  auto* passesToolbar = new PassToolbar( widget );
  passesToolbar->setVisible( false );
  QDockWidget* passesToolbarDock = new QDockWidget( mainWindow );
  passesToolbarDock->setWidget( passesToolbar );
  passesToolbarDock->setWindowTitle( passesToolbar->windowTitle() );
  passesToolbarDock->setObjectName( QStringLiteral( "PassesToolbar" ) );
  mainWindow->addDockWidget( Qt::LeftDockWidgetArea, passesToolbarDock );
  mainWindow->tabifyDockWidget( cameraToolbarDock, passesToolbarDock );
  guidanceToolbar->menu->addAction( passesToolbarDock->toggleViewAction() );

  // simulator toolbar
  auto* simulatorToolbar = new SimulatorToolbar( widget );
  simulatorToolbar->setVisible( false );
  QDockWidget* simulatorToolbarDock = new QDockWidget( mainWindow );
  simulatorToolbarDock->setWidget( simulatorToolbar );
  simulatorToolbarDock->setWindowTitle( simulatorToolbar->windowTitle() );
  simulatorToolbarDock->setObjectName( QStringLiteral( "SimulatorToolbar" ) );
  simulatorToolbarDock->setFeatures( QDockWidget::DockWidgetMovable |
                                     QDockWidget::DockWidgetFloatable |
                                     QDockWidget::DockWidgetVerticalTitleBar );
  simulatorToolbarDock->setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
  mainWindow->addDockWidget( Qt::BottomDockWidgetArea, simulatorToolbarDock );

  // XTE dock
  BlockFactory* xteBarModelFactory = new XteBarModelFactory(
    mainWindow,
    Qt::TopDockWidgetArea,
    Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea,
    QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar,
    guidanceToolbar->menu );
  xteBarModelFactory->addToCombobox( settingDialog->getCbNodeType() );

  // meter dock
  BlockFactory* meterBarModelFactory = new MeterBarModelFactory(
    mainWindow,
    Qt::TopDockWidgetArea,
    Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea,
    QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar,
    guidanceToolbar->menu );
  meterBarModelFactory->addToCombobox( settingDialog->getCbNodeType() );

  // section controll dock
  BlockFactory* sectionControlFactory = new SectionControlModelFactory(
    mainWindow,
    Qt::BottomDockWidgetArea,
    Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea,
    QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar,
    guidanceToolbar->menu,
    simulatorToolbarDock );
  sectionControlFactory->addToCombobox( settingDialog->getCbNodeType() );

  // camera block
  BlockFactory* cameraControllerFactory = new CameraControllerFactory( rootEntity, cameraEntity );
  BlockBase* cameraController = cameraControllerFactory->createNewObject();
  cameraControllerFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), cameraController );
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  view->installEventFilter( cameraController );

  // grid block
  BlockFactory* gridModelFactory = new GridModelFactory( rootEntity, cameraEntity );
  BlockBase* gridModel = gridModelFactory->createNewObject();
  gridModelFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), gridModel );

  // FPS measuremend block
  BlockFactory* fpsMeasurementFactory = new FpsMeasurementFactory( rootEntity );
  BlockBase* fpsMeasurement = fpsMeasurementFactory->createNewObject();
  fpsMeasurementFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView(), fpsMeasurement );

  // GUI -> GUI
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged,
                    simulatorToolbarDock, &QWidget::setVisible );
  QObject::connect( guidanceToolbar, &GuidanceToolbar::toggleSettings,
                    settingDialog, &SettingsDialog::toggleVisibility );

  // camera dock -> camera controller
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

  // settings dialog -> grid model
  QObject::connect( settingDialog, SIGNAL( setGrid( bool ) ),
                    gridModel, SLOT( setGrid( bool ) ) );
  QObject::connect( settingDialog, SIGNAL( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ),
                    gridModel, SLOT( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ) );

  // connect the signals of the simulator
  QObject::connect( guidanceToolbar, SIGNAL( simulatorChanged( bool ) ),
                    settingDialog->poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( velocityChanged( float ) ),
                    settingDialog->poseSimulation, SLOT( setVelocity( float ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( frequencyChanged( int ) ),
                    settingDialog->poseSimulation, SLOT( setFrequency( int ) ) );
  QObject::connect( simulatorToolbar, SIGNAL( steerangleChanged( float ) ),
                    settingDialog->poseSimulation, SLOT( setSteerAngle( float ) ) );

  // guidance dock -> settings dialog
  QObject::connect( guidanceToolbar, SIGNAL( a_clicked() ),
                    settingDialog->plannerGui, SIGNAL( a_clicked() ) );
  QObject::connect( guidanceToolbar, SIGNAL( b_clicked() ),
                    settingDialog->plannerGui, SIGNAL( b_clicked() ) );
  QObject::connect( guidanceToolbar, SIGNAL( snap_clicked() ),
                    settingDialog->plannerGui, SIGNAL( snap_clicked() ) );
  QObject::connect( guidanceToolbar, SIGNAL( autosteerEnabled( bool ) ),
                    settingDialog->plannerGui, SIGNAL( autosteerEnabled( bool ) ) );

  // turning dock -> settings dialog
  QObject::connect( turningToolbar, SIGNAL( turnLeft() ),
                    settingDialog->plannerGui, SIGNAL( turnLeft_clicked() ) );
  QObject::connect( turningToolbar, SIGNAL( turnRight() ),
                    settingDialog->plannerGui, SIGNAL( turnRight_clicked() ) );

  // passes dock -> settings dialog
  QObject::connect( passesToolbar, SIGNAL( passSettingsChanged( int, int, bool, bool ) ),
                    settingDialog->globalPlanner, SLOT( setPassSettings( int, int, bool, bool ) ) );
  QObject::connect( passesToolbar, SIGNAL( passNumberChanged( int ) ),
                    settingDialog->globalPlanner, SLOT( setPassNumber( int ) ) );

  // emit all initial signals from the settings dialog
  settingDialog->emitAllConfigSignals();

  // load states of checkboxes from global config
  {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    guidanceToolbar->cbSimulatorSetChecked( settings.value( "RunSimulatorOnStart", false ).toBool() );

    if( settings.value( "OpenSettingsDialogOnStart", false ).toBool() ) {
      settingDialog->show();
    }
  }

  // start all the tasks of settingDialog on start/exit
  settingDialog->onStart();
  QObject::connect( mainWindow, &MainWindow::closed,
                    settingDialog, &SettingsDialog::onExit );

  // camera controller
  QObject::connect( mainWindow, SIGNAL( closed() ),
                    cameraController, SLOT( saveValuesToConfig() ) );

  // Show window
#ifdef Q_OS_ANDROID
  mainWindow->showMaximized();
#else
  mainWindow->show();
  mainWindow->resize( 1200, 800 );
#endif

  return QApplication::exec();
}

