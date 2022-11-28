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

#include <QAction>
#include <QEvent>
#include <QGuiApplication>
#include <QMenu>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include <ThreadWeaver/ThreadWeaver>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QDebugOverlay>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFrustumCulling>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QMesh>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTexture>

#include <Qt3DRender/QLayer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QSortPolicy>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QRenderAspect>

#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/Qt3DWindow>

#include "gui/CameraToolbar.h"
#include "gui/FieldsOptimitionToolbar.h"
#include "gui/FieldsToolbar.h"
#include "gui/GuidanceToolbar.h"
#include "gui/GuidanceTurningToolbar.h"
#include "gui/MyMainWindow.h"
#include "gui/NewOpenSaveToolbar.h"
#include "gui/PassToolbar.h"
#include "gui/SettingsDialog.h"
#include "gui/dock/SliderDock.h"

#include "block/global/CameraController.h"
#include "block/global/FieldManager.h"
#include "block/global/FpsMeasurement.h"
#include "block/global/GridModel.h"
#include "block/graphical/TractorModel.h"
#include "block/graphical/TrailerModel.h"

#include "block/sectionControl/Implement.h"

#include "block/dock/display/OrientationDockBlock.h"
#include "block/dock/display/PositionDockBlock.h"
#include "block/dock/display/ValueDockBlock.h"
#include "block/dock/display/XteDockBlock.h"

#include "block/dock/plot/OrientationPlotDockBlock.h"
#include "block/dock/plot/ValuePlotDockBlock.h"

#include "block/dock/input/ActionDockBlock.h"
#include "block/dock/input/SliderDockBlock.h"

#include "block/global/PoseSimulation.h"
#include "block/guidance/PoseSynchroniser.h"

#include "block/kinematic/FixedKinematic.h"
#include "block/kinematic/TrailerKinematic.h"
#include "kinematic/Plan.h"
#include "kinematic/PlanGlobal.h"

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"

#include <kddockwidgets/Config.h>
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

#include "gui/MyFrameworkWidgetFactory.h"

#if defined( Q_OS_ANDROID )
  #include <QtAndroid>
const std::vector< QString >
  permissions( { "android.permission.INTERNET", "android.permission.WRITE_EXTERNAL_STORAGE", "android.permission.READ_EXTERNAL_STORAGE" } );
#endif

void
registerEigenTypes() {
  qRegisterMetaType< float >();
  qRegisterMetaType< double >();
  qRegisterMetaType< CalculationOption::Options >();

  qRegisterMetaType< Eigen::Vector2d >();
  qRegisterMetaType< Eigen::Vector3d >();
  qRegisterMetaType< Eigen::Vector4d >();

  qRegisterMetaType< Eigen::Quaterniond >();

  qRegisterMetaType< std::shared_ptr< std::vector< double > > >();
}

int
main( int argc, char** argv ) {
  //  // hack to make the app apear without cropped qt3d-widget
  //  QCoreApplication::setAttribute( Qt::AA_DisableHighDpiScaling );

  QGuiApplication::setHighDpiScaleFactorRoundingPolicy( Qt::HighDpiScaleFactorRoundingPolicy::PassThrough );
  //  QCoreApplication::setAttribute( Qt::AA_Use96Dpi );

  // make qDebug() more expressive
  //  qSetMessagePattern( "%{file}:%{line}, %{function}: %{message}" );

  //  QApplication app( argc, argv );
  QApplication app( argc, argv );
  QApplication::setOrganizationDomain( QStringLiteral( "QtOpenGuidance.org" ) );
  QApplication::setApplicationName( QStringLiteral( "QtOpenGuidance" ) );

  QThread* calculationsThread = new QThread();
  calculationsThread->start();
  QThread* guiThread  = QApplication::instance()->thread();
  QThread* qt3dThread = QApplication::instance()->thread();

  // access the threadweaver from the main thread to create the global thread pool
  ThreadWeaver::Queue::instance();

#if !defined( Q_OS_LINUX ) || defined( Q_OS_ANDROID )
  QIcon::setThemeSearchPaths( QIcon::themeSearchPaths() << QStringLiteral( ":themes/" ) );
  QIcon::setThemeName( QStringLiteral( "oxygen" ) );
#endif

  // Request required permissions at runtime on android
#ifdef Q_OS_ANDROID

  for( const auto& permission : permissions ) {
    auto result = QtAndroid::checkPermission( permission );

    if( result == QtAndroid::PermissionResult::Denied ) {
      auto resultHash = QtAndroid::requestPermissionsSync( QStringList( { permission } ) );

      if( resultHash[permission] == QtAndroid::PermissionResult::Denied )
        return 0;
    }
  }

#endif

  auto* view = new Qt3DExtras::Qt3DWindow();

  qDebug() << "OpenGL: " << view->format().majorVersion() << view->format().minorVersion();

  registerEigenTypes();
  registerCgalTypes();

  QWidget* container = QWidget::createWindowContainer( view );
  //  QSize screenSize = view->screen()->size();
  //  container->setMinimumSize( QSize( 500, 400 ) );
  //  container->setMaximumSize( screenSize );
  container->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

  // create MainWindow and set the parameters for the docks
  KDDockWidgets::MainWindowOptions options = KDDockWidgets::MainWindowOption_HasCentralFrame;
  auto                             flags   = KDDockWidgets::Config::self().flags();
  flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
  flags |= KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;
  qDebug() << flags;
  KDDockWidgets::Config::self().setFlags( flags );

  KDDockWidgets::Config::self().setFrameworkWidgetFactory( new CustomWidgetFactory() ); // Sets our custom factory
  //  KDDockWidgets::Config::self().setMinimumSizeOfWidgets( 10, 10 );

  auto* mainWindow = new MyMainWindow( QStringLiteral( "QtOpenGuidance" ), options );
  mainWindow->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );
  app.setWindowIcon( QIcon::fromTheme( QStringLiteral( "QtOpenGuidance" ) ) );

  auto* centralDock = new KDDockWidgets::DockWidget( QStringLiteral( "GuidanceView" ), KDDockWidgets::DockWidget::Option_NotClosable );
  centralDock->setWidget( container );
  centralDock->setTitle( QStringLiteral( "Guidance View" ) );
  mainWindow->addDockWidgetAsTab( centralDock );

  auto* widget = new QWidget( mainWindow );

  // Root entity for Qt3D
  auto* rootRootEntity = new Qt3DCore::QEntity();

  auto* backgroundEntity   = new Qt3DCore::QEntity( rootRootEntity );
  auto* middlegroundEntity = new Qt3DCore::QEntity( rootRootEntity );
  auto* foregroundEntity   = new Qt3DCore::QEntity( rootRootEntity );

  auto* backgroundRenderingLayer   = new Qt3DRender::QLayer( backgroundEntity );
  auto* middlegroundRenderingLayer = new Qt3DRender::QLayer( middlegroundEntity );
  auto* foregroundRenderingLayer   = new Qt3DRender::QLayer( foregroundEntity );

  backgroundRenderingLayer->setRecursive( true );
  middlegroundRenderingLayer->setRecursive( true );
  foregroundRenderingLayer->setRecursive( true );

  backgroundEntity->addComponent( backgroundRenderingLayer );
  middlegroundEntity->addComponent( middlegroundRenderingLayer );
  foregroundEntity->addComponent( foregroundRenderingLayer );

  // guidance toolbar
  auto* guidanceToolbar = new GuidanceToolbar( widget );
  auto* guidaceToolbarDock =
    new KDDockWidgets::DockWidget( QStringLiteral( "GuidaceToolbarDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  guidaceToolbarDock->setWidget( guidanceToolbar );
  guidaceToolbarDock->setTitle( guidanceToolbar->windowTitle() );
  mainWindow->addDockWidget( guidaceToolbarDock, KDDockWidgets::Location_OnRight );

  // Create setting Window
  auto* settingDialog = new SettingsDialog(
    foregroundEntity, middlegroundEntity, backgroundEntity, mainWindow, view, guidanceToolbar->menu, calculationsThread, widget );

  // Camera
  Qt3DRender::QCamera* cameraEntity = view->camera();

  cameraEntity->lens()->setProjectionType( Qt3DRender::QCameraLens::PerspectiveProjection );
  cameraEntity->lens()->setNearPlane( 1.0f );
  cameraEntity->lens()->setFarPlane( 8000 );
  cameraEntity->setPosition( QVector3D( 0, 0, 20.0f ) );
  cameraEntity->setUpVector( QVector3D( 0, 1, 0 ) );
  cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  cameraEntity->rollAboutViewCenter( -90 );
  cameraEntity->tiltAboutViewCenter( -45 );

  // Set root object of the scene
  view->setRootEntity( rootRootEntity );

  view->defaultFrameGraph()->setClearColor( QColor( 0x4d, 0x4d, 0x4f ) );
  view->defaultFrameGraph()->setFrustumCullingEnabled( true );

  //  // sort the QT3D objects, so transparency works
  //  Qt3DRender::QFrameGraphNode* framegraph = view->activeFrameGraph();
  //  auto* sortPolicy = new Qt3DRender::QSortPolicy();
  //  framegraph->setParent( sortPolicy );
  //  QVector<Qt3DRender::QSortPolicy::SortType> sortTypes;
  //  sortTypes << Qt3DRender::QSortPolicy::BackToFront;
  //  sortPolicy->setSortTypes( sortTypes );
  //  view->setActiveFrameGraph( sortPolicy );

  // get the varioud children of the default framegraph
  auto*                        lastChildOfFrameGraph = view->activeFrameGraph();
  Qt3DRender::QFrameGraphNode *cameraSelector, *clearBuffers, *frustumCulling, *debugOverlay;

  while( true ) {
    if( !lastChildOfFrameGraph->children().empty() ) {
      lastChildOfFrameGraph = static_cast< Qt3DRender::QFrameGraphNode* >( lastChildOfFrameGraph->children().front() );

      if( qobject_cast< Qt3DRender::QCameraSelector* >( lastChildOfFrameGraph ) != nullptr ) {
        cameraSelector = lastChildOfFrameGraph;
      }
      if( qobject_cast< Qt3DRender::QClearBuffers* >( lastChildOfFrameGraph ) != nullptr ) {
        clearBuffers = lastChildOfFrameGraph;
      }
      if( qobject_cast< Qt3DRender::QFrustumCulling* >( lastChildOfFrameGraph ) != nullptr ) {
        frustumCulling = lastChildOfFrameGraph;
      }
      if( qobject_cast< Qt3DRender::QDebugOverlay* >( lastChildOfFrameGraph ) != nullptr ) {
        debugOverlay = lastChildOfFrameGraph;
      }

    } else {
      break;
    }
  }

  // reordering the framegraph
  /*if( false ) */ {
    // add the layer to enable rendering in the fore/middle/background

    frustumCulling->setParent( cameraSelector );

    auto* noDrawClearBuffers = new Qt3DRender::QNoDraw( clearBuffers );

    auto clearBufferType = Qt3DRender::QClearBuffers::DepthBuffer;

    auto* clearBuffersForBackground = new Qt3DRender::QClearBuffers( frustumCulling );
    clearBuffersForBackground->setBuffers( clearBufferType );
    auto* backgroundRenderingLayerFilter = new Qt3DRender::QLayerFilter( clearBuffersForBackground );
    backgroundRenderingLayerFilter->addLayer( foregroundRenderingLayer );
    backgroundRenderingLayerFilter->addLayer( middlegroundRenderingLayer );
    //    backgroundRenderingLayerFilter->addLayer( backgroundRenderingLayer );
    backgroundRenderingLayerFilter->setFilterMode( Qt3DRender::QLayerFilter::FilterMode::DiscardAnyMatchingLayers );
    //    auto* noDrawForBackground = new Qt3DRender::QNoDraw( backgroundRenderingLayerFilter );

    auto* clearBuffersForMiddleground = new Qt3DRender::QClearBuffers( frustumCulling );
    clearBuffersForMiddleground->setBuffers( clearBufferType );
    auto* middlegroundRenderingLayerFilter = new Qt3DRender::QLayerFilter( clearBuffersForMiddleground );
    middlegroundRenderingLayerFilter->addLayer( foregroundRenderingLayer );
    //    middlegroundRenderingLayerFilter->addLayer( middlegroundRenderingLayer );
    middlegroundRenderingLayerFilter->addLayer( backgroundRenderingLayer );
    middlegroundRenderingLayerFilter->setFilterMode( Qt3DRender::QLayerFilter::FilterMode::DiscardAnyMatchingLayers );
    //    auto* noDrawForMiddleground = new Qt3DRender::QNoDraw( middlegroundRenderingLayerFilter );

    auto* clearBuffersForForeground = new Qt3DRender::QClearBuffers( frustumCulling );
    clearBuffersForForeground->setBuffers( clearBufferType );
    auto* foregroundRenderingLayerFilter = new Qt3DRender::QLayerFilter( clearBuffersForForeground );
    //    foregroundRenderingLayerFilter->addLayer( foregroundRenderingLayer );
    foregroundRenderingLayerFilter->addLayer( middlegroundRenderingLayer );
    foregroundRenderingLayerFilter->addLayer( backgroundRenderingLayer );
    foregroundRenderingLayerFilter->setFilterMode( Qt3DRender::QLayerFilter::FilterMode::DiscardAnyMatchingLayers );
    //    auto* noDrawForForeground = new Qt3DRender::QNoDraw( foregroundRenderingLayerFilter );

    debugOverlay->setParent( foregroundRenderingLayerFilter );
  }

  view->activeFrameGraph()->dumpObjectTree();

  //  foregroundRenderingLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::FilterMode::DiscardAnyMatchingLayers);

  //  auto* sortPolicy = new Qt3DRender::QSortPolicy(framegraph);

  // camera Toolbar
  auto* cameraToolbar     = new CameraToolbar( widget );
  auto* cameraToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "CameraToolbarDock" ) );
  cameraToolbarDock->setWidget( cameraToolbar );
  cameraToolbarDock->setTitle( cameraToolbar->windowTitle() );
  mainWindow->addDockWidget( cameraToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( cameraToolbarDock->toggleAction() );

  // passes toolbar
  auto* passesToolbar     = new PassToolbar( widget );
  auto* passesToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "PassesToolbarDock" ) );
  passesToolbarDock->setWidget( passesToolbar );
  passesToolbarDock->setTitle( passesToolbar->windowTitle() );
  mainWindow->addDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
  //  mainWindow->tabifyDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( passesToolbarDock->toggleAction() );

  // fields toolbar
  auto* fieldsToolbar     = new FieldsToolbar( widget );
  auto* fieldsToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "FieldsToolbarDock" ) );
  fieldsToolbarDock->setWidget( fieldsToolbar );
  fieldsToolbarDock->setTitle( fieldsToolbar->windowTitle() );
  mainWindow->addDockWidget( fieldsToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsToolbarDock->toggleAction() );

  // fields optimition toolbar
  auto* fieldsOptimitionToolbar     = new FieldsOptimitionToolbar( widget );
  auto* fieldsOptimitionToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "FieldsOptimitionToolbarDock" ) );
  fieldsOptimitionToolbarDock->setWidget( fieldsOptimitionToolbar );
  fieldsOptimitionToolbarDock->setTitle( fieldsOptimitionToolbar->windowTitle() );
  mainWindow->addDockWidget( fieldsOptimitionToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsOptimitionToolbarDock->toggleAction() );

  // simulator docks
  auto* simulatorVelocity = new SliderDock( widget );
  auto* simulatorVelocityDock =
    new KDDockWidgets::DockWidget( QStringLiteral( "SimulatorVelocityDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  simulatorVelocityDock->setWidget( simulatorVelocity );
  simulatorVelocityDock->setTitle( QStringLiteral( "Simulator Velocity" ) );
  simulatorVelocity->setDecimals( 1 );
  simulatorVelocity->setMaximum( 15 );
  simulatorVelocity->setMinimum( -15 );
  simulatorVelocity->setDefaultValue( 0 );
  simulatorVelocity->setUnit( QStringLiteral( " m/s" ) );
  mainWindow->addDockWidget( simulatorVelocityDock, KDDockWidgets::Location_OnBottom );
  guidanceToolbar->menu->addAction( simulatorVelocityDock->toggleAction() );

  auto* simulatorSteeringAngle = new SliderDock( widget );
  auto* simulatorSteeringAngleDock =
    new KDDockWidgets::DockWidget( QStringLiteral( "SimulatorSteeringAngleDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  simulatorSteeringAngleDock->setWidget( simulatorSteeringAngle );
  simulatorSteeringAngleDock->setTitle( QStringLiteral( "Simulator Steering Angle" ) );
  simulatorSteeringAngle->setDecimals( 1 );
  simulatorSteeringAngle->setMaximum( 40 );
  simulatorSteeringAngle->setMinimum( -40 );
  simulatorSteeringAngle->setDefaultValue( 0 );
  simulatorSteeringAngle->setUnit( QStringLiteral( " °" ) );
  simulatorSteeringAngle->setSliderInverted( true );
  mainWindow->addDockWidget( simulatorSteeringAngleDock, KDDockWidgets::Location_OnRight, simulatorVelocityDock );
  guidanceToolbar->menu->addAction( simulatorSteeringAngleDock->toggleAction() );

  auto* simulatorFrequency = new SliderDock( widget );
  auto* simulatorFrequencyDock =
    new KDDockWidgets::DockWidget( QStringLiteral( "SimulatorFrequencyDock" ), KDDockWidgets::DockWidget::Option_NotClosable );
  simulatorFrequencyDock->setWidget( simulatorFrequency );
  simulatorFrequencyDock->setTitle( QStringLiteral( "Simulator Frequency" ) );
  simulatorFrequency->setDecimals( 0 );
  simulatorFrequency->setMaximum( 250 );
  simulatorFrequency->setMinimum( 1 );
  simulatorFrequency->setDefaultValue( 20 );
  simulatorFrequency->setUnit( QStringLiteral( " Hz" ) );
  mainWindow->addDockWidget( simulatorFrequencyDock, KDDockWidgets::Location_OnRight, simulatorSteeringAngleDock );
  guidanceToolbar->menu->addAction( simulatorFrequencyDock->toggleAction() );

  // XTE dock
  BlockFactory* xteDockBlockFactory =
    new XteDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnTop, guidanceToolbar->menu );
  xteDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // value dock
  BlockFactory* valueDockBlockFactory =
    new ValueDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  valueDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // orientation dock
  BlockFactory* orientationDockBlockFactory =
    new OrientationDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  orientationDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // position dock
  BlockFactory* positionDockBlockFactory =
    new PositionDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  positionDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // value plot dock
  BlockFactory* valuePlotDockBlockFactory =
    new ValuePlotDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  valuePlotDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // orientation plot dock
  BlockFactory* orientationPlotDockBlockFactory =
    new OrientationPlotDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  orientationPlotDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // action dock
  BlockFactory* actionDockBlockFactory =
    new ActionDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  actionDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // slider dock
  BlockFactory* sliderDockBlockFactory =
    new SliderDockBlockFactory( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  sliderDockBlockFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // implements
  auto* implementFactory = new ImplementFactory(
    qt3dThread, mainWindow, KDDockWidgets::Location_OnBottom, guidanceToolbar->menu, settingDialog->implementBlockModel );
  implementFactory->addToTreeWidget( settingDialog->getBlockTreeWidget() );

  // camera block
  BlockFactory* cameraControllerFactory = new CameraControllerFactory( qt3dThread, rootRootEntity, cameraEntity );
  auto*         cameraControllerBlock   = cameraControllerFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );
  auto*         cameraController        = qobject_cast< CameraController* >( cameraControllerBlock->objects.front() );
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  view->installEventFilter( cameraControllerBlock->objects.front() );

  // grid block
  BlockFactory* gridModelFactory = new GridModelFactory( qt3dThread, middlegroundEntity, cameraEntity );
  auto*         gridModelBlock   = gridModelFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );
  auto*         gridModel        = qobject_cast< GridModel* >( gridModelBlock->objects.front() );

  // FPS measuremend block
  BlockFactory* fpsMeasurementFactory = new FpsMeasurementFactory( qt3dThread, middlegroundEntity );
  fpsMeasurementFactory->createBlock( settingDialog->getSceneOfConfigGraphicsView() );

  // Setting Dialog
  QObject::connect( guidanceToolbar, &GuidanceToolbar::toggleSettings, settingDialog, &SettingsDialog::toggleVisibility );

  // camera dock -> camera controller
  QObject::connect( cameraToolbar, SIGNAL( zoomIn() ), cameraController, SLOT( zoomIn() ) );
  QObject::connect( cameraToolbar, SIGNAL( zoomOut() ), cameraController, SLOT( zoomOut() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltUp() ), cameraController, SLOT( tiltUp() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltDown() ), cameraController, SLOT( tiltDown() ) );
  QObject::connect( cameraToolbar, SIGNAL( panLeft() ), cameraController, SLOT( panLeft() ) );
  QObject::connect( cameraToolbar, SIGNAL( panRight() ), cameraController, SLOT( panRight() ) );
  QObject::connect( cameraToolbar, SIGNAL( resetCamera() ), cameraController, SLOT( resetCamera() ) );
  QObject::connect( cameraToolbar, SIGNAL( setMode( int ) ), cameraController, SLOT( setMode( int ) ) );
  QObject::connect( settingDialog, &SettingsDialog::cameraSmoothingChanged, cameraController, &CameraController::setCameraSmoothing );

  // settings dialog -> grid model
  QObject::connect( settingDialog, SIGNAL( setGrid( bool ) ), gridModel, SLOT( setGrid( bool ) ) );
  QObject::connect( settingDialog,
                    SIGNAL( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ),
                    gridModel,
                    SLOT( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ) );

  // Simulator Docks
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorVelocity, &QWidget::setEnabled );
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorSteeringAngle, &QWidget::setEnabled );
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorFrequency, &QWidget::setEnabled );

  // connect the signals of the simulator
  QObject::connect( guidanceToolbar, SIGNAL( simulatorChanged( bool ) ), settingDialog->poseSimulation, SLOT( setSimulation( bool ) ) );
  QObject::connect( simulatorVelocity, SIGNAL( valueChanged( double ) ), settingDialog->poseSimulation, SLOT( setVelocity( double ) ) );
  QObject::connect(
    simulatorSteeringAngle, SIGNAL( valueChanged( double ) ), settingDialog->poseSimulation, SLOT( setSteerAngle( double ) ) );
  QObject::connect( simulatorFrequency, SIGNAL( valueChanged( double ) ), settingDialog->poseSimulation, SLOT( setFrequency( double ) ) );

  // passes dock -> global planner block
  QObject::connect( passesToolbar,
                    SIGNAL( passSettingsChanged( int, int, bool, bool ) ),
                    settingDialog->globalPlanner,
                    SLOT( setPassSettings( int, int, bool, bool ) ) );
  QObject::connect( passesToolbar, SIGNAL( passNumberChanged( int ) ), settingDialog->globalPlanner, SLOT( setPassNumber( int ) ) );

  // field docks -> global planner block
  QObject::connect(
    fieldsToolbar, SIGNAL( continousRecordToggled( bool ) ), settingDialog->fieldManager, SLOT( setContinousRecord( bool ) ) );
  QObject::connect( fieldsToolbar, SIGNAL( recordPoint() ), settingDialog->fieldManager, SLOT( recordPoint() ) );
  QObject::connect( fieldsToolbar,
                    SIGNAL( recordOnEdgeOfImplementChanged( bool ) ),
                    settingDialog->fieldManager,
                    SLOT( recordOnEdgeOfImplementChanged( bool ) ) );
  QObject::connect( fieldsOptimitionToolbar,
                    SIGNAL( recalculateFieldSettingsChanged( FieldsOptimitionToolbar::AlphaType, double, double, double ) ),
                    settingDialog->fieldManager,
                    SLOT( setRecalculateFieldSettings( FieldsOptimitionToolbar::AlphaType, double, double, double ) ) );
  QObject::connect( fieldsOptimitionToolbar, SIGNAL( recalculateField() ), settingDialog->fieldManager, SLOT( recalculateField() ) );
  QObject::connect(
    settingDialog->fieldManager, SIGNAL( alphaChanged( double, double ) ), fieldsOptimitionToolbar, SLOT( setAlpha( double, double ) ) );

  // set the defaults for the simulator
  simulatorVelocity->setValue( 0 );
  simulatorSteeringAngle->setValue( 0 );
  simulatorFrequency->setValue( 20 );

  // emit all initial signals from the settings dialog
  settingDialog->emitAllConfigSignals();

  // load states of checkboxes from global config
  {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

    bool simulatorEnabled = settings.value( QStringLiteral( "RunSimulatorOnStart" ), false ).toBool();
    guidanceToolbar->cbSimulatorSetChecked( simulatorEnabled );
    simulatorVelocity->setEnabled( simulatorEnabled );
    simulatorSteeringAngle->setEnabled( simulatorEnabled );
    simulatorFrequency->setEnabled( simulatorEnabled );

    if( settings.value( QStringLiteral( "OpenSettingsDialogOnStart" ), false ).toBool() ) {
      settingDialog->show();
    }
  }

  // start all the tasks of settingDialog on start/exit
  settingDialog->onStart();
  QObject::connect( mainWindow, &MyMainWindow::closed, settingDialog, &SettingsDialog::onExit );

  mainWindow->layout()->update();
  mainWindow->layout()->activate();
  mainWindow->layout()->update();

  mainWindow->readSettings();

  // Show window
#ifdef Q_OS_ANDROID
  mainWindow->showMaximized();
  mainWindow->resize( mainWindow->screen()->availableSize() / mainWindow->screen()->devicePixelRatio() );
#else
  //  mainWindow->resize( 1200, 800 );
  mainWindow->show();
//  mainWindow->readSettings();
#endif

  return QApplication::exec();
}
