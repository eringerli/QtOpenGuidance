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
#include <Qt3DExtras/QMetalRoughMaterial>

#include "gui/MyMainWindow.h"
#include "gui/SettingsDialog.h"
#include "gui/GuidanceToolbar.h"
#include "gui/GuidanceTurning.h"
#include "gui/SimulatorToolbar.h"
#include "gui/CameraToolbar.h"
#include "gui/PassToolbar.h"
#include "gui/FieldsToolbar.h"
#include "gui/FieldsOptimitionToolbar.h"

#include "block/CameraController.h"
#include "block/FpsMeasurement.h"
#include "block/TractorModel.h"
#include "block/TrailerModel.h"
#include "block/GridModel.h"
#include "block/XteDockBlock.h"
#include "block/ValueDockBlock.h"
#include "block/PositionDockBlock.h"
#include "block/OrientationDockBlock.h"

#include "block/PoseSimulation.h"
#include "block/PoseSynchroniser.h"

#include "kinematic/FixedKinematic.h"
#include "kinematic/TrailerKinematic.h"

#include "aspect/fpsaspect.h"
#include "aspect/fpsmonitor.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

#include <kddockwidgets/Config.h>
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "gui/MyFrameworkWidgetFactory.h"

#if defined (Q_OS_ANDROID)
#include <QtAndroid>
const std::vector<QString> permissions( {"android.permission.INTERNET",
                                        "android.permission.WRITE_EXTERNAL_STORAGE",
                                        "android.permission.READ_EXTERNAL_STORAGE"
                                        } );
#endif

int main( int argc, char** argv ) {
//  // hack to make the app apear without cropped qt3d-widget
//  QCoreApplication::setAttribute( Qt::AA_DisableHighDpiScaling );

  QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
  QCoreApplication::setAttribute( Qt::AA_UseHighDpiPixmaps );
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy( Qt::HighDpiScaleFactorRoundingPolicy::PassThrough );
//  QCoreApplication::setAttribute( Qt::AA_Use96Dpi );

  // make qDebug() more expressive
//  qSetMessagePattern( "%{file}:%{line}, %{function}: %{message}" );

  QApplication app( argc, argv );
  QApplication::setOrganizationDomain( QStringLiteral( "QtOpenGuidance.org" ) );
  QApplication::setApplicationName( QStringLiteral( "QtOpenGuidance" ) );

#if !defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
  QIcon::setThemeSearchPaths( QIcon::themeSearchPaths() << QStringLiteral( ":themes/" ) );
  QIcon::setThemeName( QStringLiteral( "oxygen" ) );
#endif

  //Request required permissions at runtime on android
#ifdef Q_OS_ANDROID

  for( const auto& permission : permissions ) {
    auto result = QtAndroid::checkPermission( permission );

    if( result == QtAndroid::PermissionResult::Denied ) {
      auto resultHash = QtAndroid::requestPermissionsSync( QStringList( {permission} ) );

      if( resultHash[permission] == QtAndroid::PermissionResult::Denied )
        return 0;
    }
  }

#endif

  auto* view = new Qt3DExtras::Qt3DWindow();

  qDebug() << "DPI: " << qApp->desktop()->logicalDpiX() << qApp->desktop()->logicalDpiY() << qApp->desktop()->devicePixelRatioF() << qApp->desktop()->widthMM() << qApp->desktop()->heightMM();


  QWidget* container = QWidget::createWindowContainer( view );
//  QSize screenSize = view->screen()->size();
//  container->setMinimumSize( QSize( 500, 400 ) );
//  container->setMaximumSize( screenSize );
  container->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  // create MainWindow and set the parameters for the docks
  KDDockWidgets::MainWindowOptions options = KDDockWidgets::MainWindowOption_HasCentralFrame;
  auto flags = KDDockWidgets::Config::self().flags();
  flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
  flags |= KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;
  qDebug() << flags;
  KDDockWidgets::Config::self().setFlags( flags );

  KDDockWidgets::Config::self().setFrameworkWidgetFactory( new CustomWidgetFactory() ); // Sets our custom factory

  auto* mainWindow = new MyMainWindow( QStringLiteral( "QtOpenGuidance" ), options );
  mainWindow->setWindowTitle( "QtOpenGuidance" );

  auto* centralDock = new KDDockWidgets::DockWidget( QStringLiteral( "GuidanceView" ), KDDockWidgets::DockWidget::Option_NotClosable );
  centralDock->setWidget( container );
  centralDock->setTitle( QStringLiteral( "Guidance View" ) );
  mainWindow->addDockWidgetAsTab( centralDock );

  auto* widget = new QWidget( mainWindow );
//  mainWindow->setCentralWidget( container );
//  mainWindow->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );
//  mainWindow->setDockOptions( QMainWindow::AnimatedDocks |
//                              QMainWindow::AllowNestedDocks |
//                              QMainWindow::AllowTabbedDocks |
//                              QMainWindow::VerticalTabs );
//  mainWindow->setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
//  mainWindow->setCorner( Qt::BottomLeftCorner, Qt::BottomDockWidgetArea );
//  mainWindow->setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
//  mainWindow->setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

  // Root entity for Qt3D
  auto rootEntity = new Qt3DCore::QEntity();

  // Create setting Window
  auto* settingDialog = new SettingsDialog( rootEntity, mainWindow, widget );

  // FPS measuring: register the aspect
  view->registerAspect( new FpsAspect );

//  auto* input = new Qt3DInput::QInputAspect;
//  view->registerAspect( input );

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

  cameraEntity->lens()->setProjectionType( Qt3DRender::QCameraLens::PerspectiveProjection );
  cameraEntity->lens()->setNearPlane( 1.0f );
  cameraEntity->lens()->setFarPlane( 2000 );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( true ) {
    constexpr float metalness = 0.1f;
    constexpr float roughness = 0.5f;

    auto* xAxis = new Qt3DCore::QEntity( rootEntity );
    auto* yAxis = new Qt3DCore::QEntity( rootEntity );
    auto* zAxis = new Qt3DCore::QEntity( rootEntity );

    auto* cylinderMesh = new Qt3DExtras::QCylinderMesh( xAxis );
    cylinderMesh->setRadius( 0.2f );
    cylinderMesh->setLength( 10.0f );
    cylinderMesh->setRings( 10.0f );
    cylinderMesh->setSlices( 10.0f );

    auto* blueMaterial = new Qt3DExtras::QMetalRoughMaterial( xAxis );
    blueMaterial->setBaseColor( QColor( Qt::blue ) );
    blueMaterial->setMetalness( metalness );
    blueMaterial->setRoughness( roughness );
    auto* redMaterial = new Qt3DExtras::QMetalRoughMaterial( yAxis );
    redMaterial->setBaseColor( QColor( Qt::red ) );
    redMaterial->setMetalness( metalness );
    redMaterial->setRoughness( roughness );
    auto* greenMaterial = new Qt3DExtras::QMetalRoughMaterial( zAxis );
    greenMaterial->setBaseColor( QColor( Qt::green ) );
    greenMaterial->setMetalness( metalness );
    greenMaterial->setRoughness( roughness );

    auto* xTransform = new Qt3DCore::QTransform( xAxis );
    xTransform->setTranslation( QVector3D( cylinderMesh->length() / 2, 0, 0 ) );
    xTransform->setRotationZ( 90 );
//    xTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 90 ) );
    auto* yTransform = new Qt3DCore::QTransform( yAxis );
    yTransform->setTranslation( QVector3D( 0, cylinderMesh->length() / 2, 0 ) );
    auto* zTransform = new Qt3DCore::QTransform( zAxis );
    zTransform->setTranslation( QVector3D( 0, 0, cylinderMesh->length() / 2 ) );
    zTransform->setRotationX( 90 );

//    zTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 1, 0, 0 ), 90 ) );

    xAxis->addComponent( cylinderMesh );
    xAxis->addComponent( redMaterial );
    xAxis->addComponent( xTransform );
    yAxis->addComponent( cylinderMesh );
    yAxis->addComponent( greenMaterial );
    yAxis->addComponent( yTransform );
    zAxis->addComponent( cylinderMesh );
    zAxis->addComponent( blueMaterial );
    zAxis->addComponent( zTransform );
  }

  // Set root object of the scene
  view->setRootEntity( rootEntity );

  view->defaultFrameGraph()->setClearColor( QColor( 0x4d, 0x4d, 0x4f ) );
  view->defaultFrameGraph()->setFrustumCullingEnabled( false );
  view->defaultFrameGraph()->setGamma( 2.0f );

//  // sort the QT3D objects, so transparency works
//  Qt3DRender::QFrameGraphNode* framegraph = view->activeFrameGraph();
//  auto* sortPolicy = new Qt3DRender::QSortPolicy();
//  framegraph->setParent( sortPolicy );
//  QVector<Qt3DRender::QSortPolicy::SortType> sortTypes;
//  sortTypes << Qt3DRender::QSortPolicy::BackToFront;
//  sortPolicy->setSortTypes( sortTypes );
//  view->setActiveFrameGraph( sortPolicy );

  // guidance toolbar
  auto* guidanceToolbar = new GuidanceToolbar( widget );
  auto* guidaceToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "GuidaceToolbarDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  guidaceToolbarDock->setWidget( guidanceToolbar );
  guidaceToolbarDock->setTitle( guidanceToolbar->windowTitle() );
  mainWindow->addDockWidget( guidaceToolbarDock, KDDockWidgets::Location_OnRight );

  // turning Toolbar
  auto* turningToolbar = new GuidanceTurning( mainWindow );
  auto* turningToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "TurningToolbarDock" ) );
  turningToolbarDock->setWidget( turningToolbar );
  turningToolbarDock->setTitle( turningToolbar->windowTitle() );
  mainWindow->addDockWidget( turningToolbarDock, KDDockWidgets::Location_OnTop );
  guidanceToolbar->menu->addAction( turningToolbarDock->toggleAction() );

  // camera Toolbar
  auto* cameraToolbar = new CameraToolbar( widget );
  auto* cameraToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "CameraToolbarDock" ) );
  cameraToolbarDock->setWidget( cameraToolbar );
  cameraToolbarDock->setTitle( cameraToolbar->windowTitle() );
  mainWindow->addDockWidget( cameraToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( cameraToolbarDock->toggleAction() );

  // passes toolbar
  auto* passesToolbar = new PassToolbar( widget );
  auto* passesToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "PassesToolbarDock" ) );
  passesToolbarDock->setWidget( passesToolbar );
  passesToolbarDock->setTitle( passesToolbar->windowTitle() );
  mainWindow->addDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
//  mainWindow->tabifyDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( passesToolbarDock->toggleAction() );

  // fields toolbar
  auto* fieldsToolbar = new FieldsToolbar( widget );
  auto* fieldsToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "FieldsToolbarDock" ) );
  fieldsToolbarDock->setWidget( fieldsToolbar );
  fieldsToolbarDock->setTitle( fieldsToolbar->windowTitle() );
//  QObject::connect( fieldsToolbarDock, &QDockWidget::dockLocationChanged, fieldsToolbar, &FieldsToolbar::setDockLocation );
  mainWindow->addDockWidget( fieldsToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsToolbarDock->toggleAction() );

  // fields optimition toolbar
  auto* fieldsOptimitionToolbar = new FieldsOptimitionToolbar( widget );
  auto* fieldsOptimitionToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "FieldsOptimitionToolbarDock" ) );
  fieldsOptimitionToolbarDock->setWidget( fieldsOptimitionToolbar );
  fieldsOptimitionToolbarDock->setTitle( fieldsOptimitionToolbar->windowTitle() );
  mainWindow->addDockWidget( fieldsOptimitionToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsOptimitionToolbarDock->toggleAction() );

  // simulator toolbar
  auto* simulatorToolbar = new SimulatorToolbar( widget );
  auto* simulatorToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "SimulatorToolbarDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  simulatorToolbarDock->setWidget( simulatorToolbar );
  simulatorToolbarDock->setTitle( simulatorToolbar->windowTitle() );
  mainWindow->addDockWidget( simulatorToolbarDock, KDDockWidgets::Location_OnBottom );

  // XTE dock
  BlockFactory* xteDockBlockFactory = new XteDockBlockFactory(
    mainWindow,
    KDDockWidgets::Location_OnTop,
    guidanceToolbar->menu );
  xteDockBlockFactory->addToCombobox( settingDialog->getCbNodeType() );

  // value dock
  BlockFactory* valueDockBlockFactory = new ValueDockBlockFactory(
    mainWindow,
    KDDockWidgets::Location_OnRight,
    guidanceToolbar->menu );
  valueDockBlockFactory->addToCombobox( settingDialog->getCbNodeType() );

  // orientation dock
  BlockFactory* orientationDockBlockFactory = new OrientationDockBlockFactory(
    mainWindow,
    KDDockWidgets::Location_OnRight,
    guidanceToolbar->menu );
  orientationDockBlockFactory->addToCombobox( settingDialog->getCbNodeType() );

  // orientation dock
  BlockFactory* positionDockBlockFactory = new PositionDockBlockFactory(
    mainWindow,
    KDDockWidgets::Location_OnRight,
    guidanceToolbar->menu );
  positionDockBlockFactory->addToCombobox( settingDialog->getCbNodeType() );

  // implements
  auto* implementFactory = new ImplementFactory(
    mainWindow,
    KDDockWidgets::Location_OnBottom,
    guidanceToolbar->menu,
    settingDialog->implementBlockModel );
  implementFactory->addToCombobox( settingDialog->getCbNodeType() );

  // camera block
  BlockFactory* cameraControllerFactory = new CameraControllerFactory( rootEntity, cameraEntity );
  auto* cameraControllerBlock = cameraControllerFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );
  auto* cameraController = qobject_cast<CameraController*>( cameraControllerBlock->object );
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  view->installEventFilter( cameraControllerBlock->object );

  // grid block
  BlockFactory* gridModelFactory = new GridModelFactory( rootEntity, cameraEntity );
  auto* gridModelBlock = gridModelFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );
  auto* gridModel = qobject_cast<GridModel*>( gridModelBlock->object );

  // FPS measuremend block
  BlockFactory* fpsMeasurementFactory = new FpsMeasurementFactory( rootEntity );
  fpsMeasurementFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );

  // GUI -> GUI
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged,
                    simulatorToolbarDock, &QWidget::setEnabled );
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

  // turning dock -> planner gui block
  QObject::connect( turningToolbar, SIGNAL( turnLeft() ),
                    settingDialog->plannerGui, SIGNAL( turnLeft_clicked() ) );
  QObject::connect( turningToolbar, SIGNAL( turnRight() ),
                    settingDialog->plannerGui, SIGNAL( turnRight_clicked() ) );

  // passes dock -> global planner block
  QObject::connect( passesToolbar, SIGNAL( passSettingsChanged( int, int, bool, bool ) ),
                    settingDialog->globalPlanner, SLOT( setPassSettings( int, int, bool, bool ) ) );
  QObject::connect( passesToolbar, SIGNAL( passNumberChanged( int ) ),
                    settingDialog->globalPlanner, SLOT( setPassNumber( int ) ) );

  // field docks -> global planner block
  QObject::connect( fieldsToolbar, SIGNAL( openField() ),
                    settingDialog->globalPlanner, SLOT( openField() ) );
  QObject::connect( fieldsToolbar, SIGNAL( newField() ),
                    settingDialog->globalPlanner, SLOT( newField() ) );
  QObject::connect( fieldsToolbar, SIGNAL( saveField() ),
                    settingDialog->globalPlanner, SLOT( saveField() ) );
  QObject::connect( fieldsToolbar, SIGNAL( continousRecordToggled( bool ) ),
                    settingDialog->globalPlanner, SLOT( setContinousRecord( bool ) ) );
  QObject::connect( fieldsToolbar, SIGNAL( recordPoint() ),
                    settingDialog->globalPlanner, SLOT( recordPoint() ) );
  QObject::connect( fieldsToolbar, SIGNAL( recordOnEdgeOfImplementChanged( bool ) ),
                    settingDialog->globalPlanner, SLOT( recordOnEdgeOfImplementChanged( bool ) ) );
  QObject::connect( fieldsOptimitionToolbar, SIGNAL( recalculateFieldSettingsChanged( FieldsOptimitionToolbar::AlphaType, double, double, double ) ),
                    settingDialog->globalPlanner, SLOT( setRecalculateFieldSettings( FieldsOptimitionToolbar::AlphaType, double, double, double ) ) );
  QObject::connect( fieldsOptimitionToolbar, SIGNAL( recalculateField() ),
                    settingDialog->globalPlanner, SLOT( recalculateField() ) );
  QObject::connect( settingDialog->globalPlanner, SIGNAL( alphaChanged( double, double ) ),
                    fieldsOptimitionToolbar, SLOT( setAlpha( double, double ) ) );
  QObject::connect( settingDialog->globalPlanner, SIGNAL( fieldStatisticsChanged( double, double, double ) ),
                    fieldsOptimitionToolbar, SLOT( setFieldStatistics( double, double, double ) ) );

  // emit all initial signals from the settings dialog
  settingDialog->emitAllConfigSignals();

  // load states of checkboxes from global config
  {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    bool simulatorEnabled = settings.value( QStringLiteral( "RunSimulatorOnStart" ), false ).toBool();
    guidanceToolbar->cbSimulatorSetChecked( simulatorEnabled );
    simulatorToolbarDock->setVisible( simulatorEnabled );

    if( settings.value( QStringLiteral( "OpenSettingsDialogOnStart" ), false ).toBool() ) {
      settingDialog->show();
    }
  }

  // start all the tasks of settingDialog on start/exit
  settingDialog->onStart();
  QObject::connect( mainWindow, &MyMainWindow::closed,
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
