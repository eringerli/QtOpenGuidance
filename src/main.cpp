// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include <Qt3DRender/QMultiSampleAntiAliasing>
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

#include "gui/thread/SdlInputPollingThread.h"
#include "gui/thread/SpaceNavigatorPollingThread.h"
#include "helpers/BlocksManager.h"
#include "helpers/FactoriesManager.h"

#include "gui/MyMainWindow.h"
#include "gui/OpenSaveHelper.h"
#include "gui/SettingsDialog.h"
#include "gui/dock/SliderDock.h"
#include "gui/toolbar/ApplicationControlToolbar.h"
#include "gui/toolbar/CameraToolbar.h"
#include "gui/toolbar/FieldsOptimitionToolbar.h"
#include "gui/toolbar/FieldsToolbar.h"
#include "gui/toolbar/GuidanceToolbar.h"
#include "gui/toolbar/NewOpenSaveToolbar.h"
#include "gui/toolbar/PassToolbar.h"

#include "block/literal/NumberBlock.h"
#include "block/literal/OrientationBlock.h"
#include "block/literal/StringBlock.h"
#include "block/literal/VectorBlock.h"

#include "block/arithmetic/ArithmeticAbsolute.h"
#include "block/arithmetic/ArithmeticAddition.h"
#include "block/arithmetic/ArithmeticClamp.h"
#include "block/arithmetic/ArithmeticDivision.h"
#include "block/arithmetic/ArithmeticMultiplication.h"
#include "block/arithmetic/ArithmeticNegation.h"
#include "block/arithmetic/ArithmeticSubtraction.h"

#include "block/comparison/ComparisonEqualTo.h"
#include "block/comparison/ComparisonGreaterOrEqualTo.h"
#include "block/comparison/ComparisonGreaterThan.h"
#include "block/comparison/ComparisonLessOrEqualTo.h"
#include "block/comparison/ComparisonLessThan.h"
#include "block/comparison/ComparisonNotEqualTo.h"

#include "block/logic/LogicDivider.h"
#include "block/logic/SchmittTriggerBlock.h"
#include "block/logic/ValveNumber.h"

#include "block/splitter/SplitterImu.h"
#include "block/splitter/SplitterOrientation.h"
#include "block/splitter/SplitterPose.h"
#include "block/splitter/SplitterVector.h"

#include "block/ratelimiter/RateLimiterImu.h"
#include "block/ratelimiter/RateLimiterOrientation.h"
#include "block/ratelimiter/RateLimiterPose.h"
#include "block/ratelimiter/RateLimiterVector.h"

#include "block/sectionControl/Implement.h"

#include "block/graphical/PathPlannerModel.h"

#include "block/global/CameraController.h"
#include "block/global/GridModel.h"

#include "block/graphical/CultivatedAreaModel.h"
#include "block/graphical/SprayerModel.h"
#include "block/graphical/TractorModel.h"
#include "block/graphical/TrailerModel.h"

#include "block/calculation/AckermannSteering.h"
#include "block/calculation/AngularVelocityLimiter.h"

#include "block/global/PoseSimulation.h"

#ifdef SPNAV_ENABLED
  #include "gui/thread/SpaceNavigatorPollingThread.h"
#endif

#ifdef SDL2_ENABLED
  #include "gui/thread/SdlInputPollingThread.h"
#endif

#include "block/filter/CascadedComplementaryFilterImuFusion.h"
#include "block/filter/ComplementaryFilterImuFusion.h"
#include "block/filter/ExtendedKalmanFilter.h"
#include "block/filter/NonLinearComplementaryFilterImuFusion.h"

#include "block/guidance/PoseSynchroniser.h"

#include "block/calculation/TransverseMercatorConverter.h"
#include "block/parser/NmeaParserGGA.h"
#include "block/parser/NmeaParserHDT.h"
#include "block/parser/NmeaParserRMC.h"
#include "block/parser/UbxParser.h"

#include "block/dock/input/ActionDockBlock.h"
#include "block/global/FieldManager.h"
#include "block/graphical/PathPlannerModel.h"
#include "block/guidance/GlobalPlanner.h"
#include "block/guidance/LocalPlanOptimizer.h"
#include "block/guidance/LocalPlanner.h"
#include "block/guidance/MpcGuidance.h"
#include "block/guidance/PoseSynchroniser.h"
#include "block/guidance/SimpleMpcGuidance.h"
#include "block/guidance/StanleyGuidance.h"
#include "block/guidance/XteGuidance.h"

#include "block/sectionControl/SectionControl.h"

#include "block/base/DebugSink.h"

#include "block/converter/CommunicationJrk.h"
#include "block/converter/CommunicationPgn7FFE.h"
#include "block/stream/FileStream.h"
#include "block/stream/UdpSocket.h"

#ifdef SERIALPORT_ENABLED
  #include "block/stream/SerialPort.h"
  #include <QSerialPortInfo>
#endif

#include "block/converter/ValueTransmissionBase64Data.h"
#include "block/converter/ValueTransmissionImuData.h"
#include "block/converter/ValueTransmissionNumber.h"
#include "block/converter/ValueTransmissionQuaternion.h"
#include "block/converter/ValueTransmissionState.h"

#include "block/kinematic/FixedKinematic.h"
#include "block/kinematic/FixedKinematicPrimitive.h"
#include "block/kinematic/TrailerKinematic.h"
#include "block/kinematic/TrailerKinematicPrimitive.h"
#include "helpers/GeographicConvertionWrapper.h"

#include "block/dock/display/OrientationDockBlock.h"
#include "block/dock/display/PositionDockBlock.h"
#include "block/dock/display/ValueDockBlock.h"
#include "block/dock/display/XteDockBlock.h"

#include "block/dock/input/ActionDockBlock.h"
#include "block/dock/input/SliderDockBlock.h"

#include "block/dock/plot/OrientationPlotDockBlock.h"
#include "block/dock/plot/ValuePlotDockBlock.h"
#include "block/dock/plot/VectorPlotDockBlock.h"

#include "block/global/FpsMeasurement.h"

#include <kddockwidgets/Config.h>
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>
#include <memory>

#include "gui/MyViewFactory.h"

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

  qRegisterMetaType< const float >();
  qRegisterMetaType< const double >();
  qRegisterMetaType< const CalculationOption::Options >();
  qRegisterMetaType< const Eigen::Vector2d >();
  qRegisterMetaType< const Eigen::Vector3d >();
  qRegisterMetaType< const Eigen::Vector4d >();
  qRegisterMetaType< const Eigen::Quaterniond >();

  qRegisterMetaType< const Eigen::Vector2d& >();
  qRegisterMetaType< const Eigen::Vector3d& >();
  qRegisterMetaType< const Eigen::Vector4d& >();
  qRegisterMetaType< const Eigen::Quaterniond& >();

  qRegisterMetaType< std::shared_ptr< std::vector< double > > >();
}

int
main( int argc, char** argv ) {
  //  // hack to make the app apear without cropped qt3d-widget
  //  QCoreApplication::setAttribute( Qt::AA_DisableHighDpiScaling );

  qputenv( "QT3D_RENDERER", "opengl" );
  // qputenv( "QT3D_RENDERER", "opengl" );
  // qputenv( "QSG_RHI_BACKEND", "vulkan" );

  QGuiApplication::setHighDpiScaleFactorRoundingPolicy( Qt::HighDpiScaleFactorRoundingPolicy::PassThrough );
  //  QCoreApplication::setAttribute( Qt::AA_Use96Dpi );

  // make qDebug() more expressive
  //  qSetMessagePattern( "%{file}:%{line}, %{function}: %{message}" );

  QApplication app( argc, argv );
  QApplication::setOrganizationDomain( QStringLiteral( "QtOpenGuidance.org" ) );
  QApplication::setApplicationName( QStringLiteral( "QtOpenGuidance" ) );

  KDDockWidgets::initFrontend( KDDockWidgets::FrontendType::QtWidgets );

  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

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

  bool usePBR = settings.value( QStringLiteral( "Material/usePBR" ), true ).toBool();

  if( view->format().majorVersion() < 2 ) {
    usePBR = false;
  }

  registerEigenTypes();
  registerCgalTypes();

  QWidget* container = QWidget::createWindowContainer( view );
  //  QSize screenSize = view->screen()->size();
  //  container->setMinimumSize( QSize( 500, 400 ) );
  //  container->setMaximumSize( screenSize );
  container->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );

  // create MainWindow and set the parameters for the docks
  KDDockWidgets::MainWindowOptions options = KDDockWidgets::MainWindowOption_None /*KDDockWidgets::MainWindowOption_HasCentralFrame*/;
  auto                             flags   = KDDockWidgets::Config::self().flags();
  flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
  //  flags |= KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;
  //  flags |= KDDockWidgets::Config::Flag_LazyResize;
  flags |= KDDockWidgets::Config::Flag_AutoHideSupport;
  flags |= KDDockWidgets::Config::Flag_TitleBarHasMinimizeButton;
  flags |= KDDockWidgets::Config::Flag_TitleBarHasMaximizeButton;
  flags |= KDDockWidgets::Config::Flag_TitleBarNoFloatButton;

  qDebug() << flags;
  KDDockWidgets::Config::self().setFlags( flags );
  //  options |= KDDockWidgets::MainWindowOption_HasCentralWidget;

  //  KDDockWidgets::Config::self().setFrameworkWidgetFactory( new CustomWidgetFactory() ); // Sets our custom factory
  //  KDDockWidgets::Config::self().setMinimumSizeOfWidgets( 10, 10 );

  auto* mainWindow = new MyMainWindow( QStringLiteral( "QtOpenGuidance" ), options );
  mainWindow->setWindowTitle( QStringLiteral( "QtOpenGuidance" ) );
  app.setWindowIcon( QIcon::fromTheme( QStringLiteral( "QtOpenGuidance" ) ) );

  auto* centralDock =
    new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "GuidanceView" ), KDDockWidgets::DockWidgetOption_NotClosable );
  centralDock->setWidget( container );
  centralDock->setTitle( QStringLiteral( "Guidance View" ) );
  mainWindow->addDockWidget( centralDock, KDDockWidgets::Location_OnRight );

  auto* widgetOfMainWindow = new QWidget( mainWindow );

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

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( true ) {
    constexpr float metalness = 0.1f;
    constexpr float roughness = 0.5f;

    auto* xAxis        = new Qt3DCore::QEntity( middlegroundEntity );
    auto* cylinderMesh = new Qt3DExtras::QCylinderMesh( xAxis );
    cylinderMesh->setRadius( 0.2f );
    cylinderMesh->setLength( 10.0f );
    cylinderMesh->setRings( 10.0f );
    cylinderMesh->setSlices( 10.0f );

    {
      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( xAxis );
        material->setBaseColor( QColor( Qt::blue ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        xAxis->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( xAxis );
        material->setDiffuse( QColor( Qt::blue ) );
        xAxis->addComponent( material );
      }

      auto* xTransform = new Qt3DCore::QTransform( xAxis );
      xTransform->setTranslation( QVector3D( cylinderMesh->length() / 2, 0, 0 ) );
      xTransform->setRotationZ( 90 );

      xAxis->addComponent( cylinderMesh );
      xAxis->addComponent( xTransform );
    }

    {
      auto* yAxis = new Qt3DCore::QEntity( middlegroundEntity );

      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( yAxis );
        material->setBaseColor( QColor( Qt::red ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        yAxis->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( yAxis );
        material->setDiffuse( QColor( Qt::red ) );
        yAxis->addComponent( material );
      }

      auto* yTransform = new Qt3DCore::QTransform( yAxis );
      yTransform->setTranslation( QVector3D( 0, cylinderMesh->length() / 2, 0 ) );

      yAxis->addComponent( cylinderMesh );
      yAxis->addComponent( yTransform );
    }

    {
      auto* zAxis = new Qt3DCore::QEntity( middlegroundEntity );

      if( usePBR ) {
        auto* material = new Qt3DExtras::QMetalRoughMaterial( zAxis );
        material->setBaseColor( QColor( Qt::green ) );
        material->setMetalness( metalness );
        material->setRoughness( roughness );
        zAxis->addComponent( material );
      } else {
        auto* material = new Qt3DExtras::QDiffuseSpecularMaterial( zAxis );
        material->setDiffuse( QColor( Qt::green ) );
        zAxis->addComponent( material );
      }

      auto* zTransform = new Qt3DCore::QTransform( zAxis );
      zTransform->setTranslation( QVector3D( 0, 0, cylinderMesh->length() / 2 ) );
      zTransform->setRotationX( 90 );

      zAxis->addComponent( cylinderMesh );
      zAxis->addComponent( zTransform );
    }
  }

  // guidance toolbar
  auto* guidanceToolbar = new GuidanceToolbar( widgetOfMainWindow );
  auto* guidaceToolbarDock =
    new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "GuidaceToolbarDock" ), KDDockWidgets::DockWidgetOption_NotClosable );
  guidaceToolbarDock->setWidget( guidanceToolbar );
  guidaceToolbarDock->setTitle( guidanceToolbar->windowTitle() );
  mainWindow->addDockWidget( guidaceToolbarDock, KDDockWidgets::Location_OnRight );

  auto* applicationControlToolbar     = new ApplicationControlToolbar( widgetOfMainWindow );
  auto* applicationControlToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "ApplicationControlToolbar" ) );
  applicationControlToolbarDock->setWidget( applicationControlToolbar );
  applicationControlToolbarDock->setTitle( applicationControlToolbar->windowTitle() );
  guidanceToolbar->menu->addAction( applicationControlToolbarDock->toggleAction() );
  mainWindow->addDockWidget( applicationControlToolbarDock, KDDockWidgets::Location_OnRight );

  FactoriesManager factoriesManager;
  BlocksManager    blocksManager( factoriesManager );

  // Create setting Window
  auto* settingsDialog = new SettingsDialog( mainWindow, view, &factoriesManager, &blocksManager, widgetOfMainWindow );

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
  view->activeFrameGraph()->dumpObjectTree();
  qDebug() << "after:";

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
  if( true ) {
    // add the layer to enable rendering in the fore/middle/background
    // filterKey->setParent( cameraSelector );

    auto* msaa = new Qt3DRender::QMultiSampleAntiAliasing( cameraSelector );

    frustumCulling->setParent( msaa );

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
    // auto* noDrawForForeground = new Qt3DRender::QNoDraw( foregroundRenderingLayerFilter );

    debugOverlay->setParent( /*noDrawForForeground*/ foregroundRenderingLayerFilter );
  }

  view->activeFrameGraph()->dumpObjectTree();

  //  foregroundRenderingLayerFilter->setFilterMode(Qt3DRender::QLayerFilter::FilterMode::DiscardAnyMatchingLayers);

  //  auto* sortPolicy = new Qt3DRender::QSortPolicy(framegraph);

  // camera Toolbar
  auto* cameraToolbar     = new CameraToolbar( widgetOfMainWindow );
  auto* cameraToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "CameraToolbarDock" ) );
  cameraToolbarDock->setWidget( cameraToolbar );
  cameraToolbarDock->setTitle( cameraToolbar->windowTitle() );
  mainWindow->addDockWidget( cameraToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( cameraToolbarDock->toggleAction() );

  // passes toolbar
  auto* passesToolbar     = new PassToolbar( widgetOfMainWindow );
  auto* passesToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "PassesToolbarDock" ) );
  passesToolbarDock->setWidget( passesToolbar );
  passesToolbarDock->setTitle( passesToolbar->windowTitle() );
  mainWindow->addDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
  //  mainWindow->tabifyDockWidget( passesToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( passesToolbarDock->toggleAction() );

  // fields toolbar
  auto* fieldsToolbar     = new FieldsToolbar( widgetOfMainWindow );
  auto* fieldsToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "FieldsToolbarDock" ) );
  fieldsToolbarDock->setWidget( fieldsToolbar );
  fieldsToolbarDock->setTitle( fieldsToolbar->windowTitle() );
  mainWindow->addDockWidget( fieldsToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsToolbarDock->toggleAction() );

  // fields optimition toolbar
  auto* fieldsOptimitionToolbar     = new FieldsOptimitionToolbar( widgetOfMainWindow );
  auto* fieldsOptimitionToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "FieldsOptimitionToolbarDock" ) );
  fieldsOptimitionToolbarDock->setWidget( fieldsOptimitionToolbar );
  fieldsOptimitionToolbarDock->setTitle( fieldsOptimitionToolbar->windowTitle() );
  mainWindow->addDockWidget( fieldsOptimitionToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( fieldsOptimitionToolbarDock->toggleAction() );

  // new/open/save toolbar
  auto* newOpenSaveToolbar     = new NewOpenSaveToolbar( widgetOfMainWindow );
  auto* newOpenSaveToolbarDock = new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "NewOpenSaveToolbarDock" ) );
  newOpenSaveToolbarDock->setWidget( newOpenSaveToolbar );
  newOpenSaveToolbarDock->setTitle( QStringLiteral( "New, Save, Open" ) );
  mainWindow->addDockWidget( newOpenSaveToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbar->menu->addAction( newOpenSaveToolbarDock->toggleAction() );

  // simulator docks
  auto* simulatorVelocity = new SliderDock( widgetOfMainWindow );
  auto* simulatorVelocityDock =
    new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "SimulatorVelocityDock" ), KDDockWidgets::DockWidgetOption_NotClosable );
  simulatorVelocityDock->setWidget( simulatorVelocity );
  simulatorVelocityDock->setTitle( QStringLiteral( "Simulator Velocity" ) );
  simulatorVelocity->setDecimals( 1 );
  simulatorVelocity->setMaximum( 15 );
  simulatorVelocity->setMinimum( -15 );
  simulatorVelocity->setDefaultValue( 0 );
  simulatorVelocity->setUnit( QStringLiteral( " m/s" ) );
  mainWindow->addDockWidget( simulatorVelocityDock, KDDockWidgets::Location_OnBottom );
  guidanceToolbar->menu->addAction( simulatorVelocityDock->toggleAction() );

  auto* simulatorSteeringAngle = new SliderDock( widgetOfMainWindow );
  auto* simulatorSteeringAngleDock =
    new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "SimulatorSteeringAngleDock" ), KDDockWidgets::DockWidgetOption_NotClosable );
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

  auto* simulatorFrequency = new SliderDock( widgetOfMainWindow );
  auto* simulatorFrequencyDock =
    new KDDockWidgets::QtWidgets::DockWidget( QStringLiteral( "SimulatorFrequencyDock" ), KDDockWidgets::DockWidgetOption_NotClosable );
  simulatorFrequencyDock->setWidget( simulatorFrequency );
  simulatorFrequencyDock->setTitle( QStringLiteral( "Simulator Frequency" ) );
  simulatorFrequency->setDecimals( 0 );
  simulatorFrequency->setMaximum( 250 );
  simulatorFrequency->setMinimum( 1 );
  simulatorFrequency->setDefaultValue( 20 );
  simulatorFrequency->setUnit( QStringLiteral( " Hz" ) );
  mainWindow->addDockWidget( simulatorFrequencyDock, KDDockWidgets::Location_OnRight, simulatorSteeringAngleDock );
  guidanceToolbar->menu->addAction( simulatorFrequencyDock->toggleAction() );

  // initialise the wrapper for the geographic conversion, so all offsets are the same
  // application-wide
  auto geographicConvertionWrapper = std::make_unique< GeographicConvertionWrapper >();

  factoriesManager.addFactory< XteDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnTop, guidanceToolbar->menu );
  factoriesManager.addFactory< ValueDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  factoriesManager.addFactory< OrientationDockBlockFactory >(
    guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  factoriesManager.addFactory< PositionDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );

  factoriesManager.addFactory< ValuePlotDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  factoriesManager.addFactory< OrientationPlotDockBlockFactory >(
    guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  factoriesManager.addFactory< VectorPlotDockBlockFactory >(
    guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );

  factoriesManager.addFactory< ActionDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );
  factoriesManager.addFactory< SliderDockBlockFactory >( guiThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu );

  factoriesManager.addFactory< ImplementFactory >(
    qt3dThread, mainWindow, KDDockWidgets::Location_OnBottom, guidanceToolbar->menu, settingsDialog->implementBlockModel );

  factoriesManager.addFactory< LocalPlannerFactory >(
    calculationsThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu, middlegroundEntity );
  factoriesManager.addFactory< LocalPlanOptimizerFactory >(
    calculationsThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu, middlegroundEntity );
  factoriesManager.addFactory< StanleyGuidanceFactory >( calculationsThread );
  factoriesManager.addFactory< SimpleMpcGuidanceFactory >(
    calculationsThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu, foregroundEntity );
  factoriesManager.addFactory< MpcGuidanceFactory >(
    calculationsThread, mainWindow, KDDockWidgets::Location_OnRight, guidanceToolbar->menu, foregroundEntity );
  factoriesManager.addFactory< XteGuidanceFactory >( calculationsThread );
  //  factoriesManager.addFactory< SectionControlFactory >(
  //    qt3dThread,
  //    mainWindow,
  //    KDDockWidgets::Location_OnBottom,
  //    guidanceToolbar->menu,
  //    backgroundEntity,
  //    static_cast< Qt3DRender::QFrameGraphNode* >( view->activeFrameGraph()->children().front() ) );

  factoriesManager.addFactory< PathPlannerModelFactory >( qt3dThread, middlegroundEntity );

  // Factories for the blocks
  factoriesManager.addFactory< TransverseMercatorConverterFactory >( calculationsThread, geographicConvertionWrapper.get() );
  factoriesManager.addFactory< PoseSynchroniserFactory >( calculationsThread );
  factoriesManager.addFactory< ExtendedKalmanFilterFactory >( calculationsThread );
  factoriesManager.addFactory< CascadedComplementaryFilterImuFusionFactory >( calculationsThread );
  factoriesManager.addFactory< ComplementaryFilterImuFusionFactory >( calculationsThread );
  factoriesManager.addFactory< NonLinearComplementaryFilterImuFusionFactory >( calculationsThread );
  factoriesManager.addFactory< TrailerModelFactory >( qt3dThread, foregroundEntity, usePBR );
  factoriesManager.addFactory< TractorModelFactory >( qt3dThread, foregroundEntity, usePBR );
  factoriesManager.addFactory< SprayerModelFactory >( qt3dThread, foregroundEntity, usePBR );
  factoriesManager.addFactory< CultivatedAreaModelFactory >( qt3dThread, middlegroundEntity, usePBR, newOpenSaveToolbar );
  factoriesManager.addFactory< FixedKinematicFactory >( calculationsThread );
  factoriesManager.addFactory< TrailerKinematicFactory >( calculationsThread );
  factoriesManager.addFactory< FixedKinematicPrimitiveFactory >( calculationsThread );
  factoriesManager.addFactory< TrailerKinematicPrimitiveFactory >( calculationsThread );
  factoriesManager.addFactory< VectorBlockFactory >( guiThread, settingsDialog->getVectorBlockModel() );

  factoriesManager.addFactory< OrientationBlockFactory >( guiThread, settingsDialog->getOrientationBlockModel() );
  factoriesManager.addFactory< NumberBlockFactory >( guiThread, settingsDialog->getNumberBlockModel() );
  factoriesManager.addFactory< StringBlockFactory >( guiThread, settingsDialog->getStringBlockModel() );
  //  factoriesManager.addFactory<DebugSink>(( guiThread ) );
  factoriesManager.addFactory< UdpSocketFactory >( guiThread );

  factoriesManager.addFactory< ArithmeticAbsoluteFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticAdditionFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticClampFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticSubtractionFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticMultiplicationFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticNegationFactory >( calculationsThread );
  factoriesManager.addFactory< ArithmeticDivisionFactory >( calculationsThread );

  factoriesManager.addFactory< ValveNumberFactory >( calculationsThread );
  factoriesManager.addFactory< SchmittTriggerBlockFactory >( calculationsThread );
  factoriesManager.addFactory< LogicDividerFactory >( calculationsThread );

  factoriesManager.addFactory< SplitterImuFactory >( calculationsThread );
  factoriesManager.addFactory< SplitterOrientationFactory >( calculationsThread );
  factoriesManager.addFactory< SplitterPoseFactory >( calculationsThread );
  factoriesManager.addFactory< SplitterVectorFactory >( calculationsThread );

  factoriesManager.addFactory< RateLimiterImuFactory >( calculationsThread );
  factoriesManager.addFactory< RateLimiterOrientationFactory >( calculationsThread );
  factoriesManager.addFactory< RateLimiterPoseFactory >( calculationsThread );
  factoriesManager.addFactory< RateLimiterVectorFactory >( calculationsThread );

  factoriesManager.addFactory< ComparisonEqualToFactory >( calculationsThread );
  factoriesManager.addFactory< ComparisonNotEqualToFactory >( calculationsThread );
  factoriesManager.addFactory< ComparisonGreaterThanFactory >( calculationsThread );
  factoriesManager.addFactory< ComparisonLessThanFactory >( calculationsThread );
  factoriesManager.addFactory< ComparisonGreaterOrEqualToFactory >( calculationsThread );
  factoriesManager.addFactory< ComparisonLessOrEqualToFactory >( calculationsThread );

#ifdef SERIALPORT_ENABLED
  factoriesManager.addFactory< SerialPortFactory >( guiThread );
#endif

  factoriesManager.addFactory< FileStreamFactory >( guiThread );
  factoriesManager.addFactory< CommunicationPgn7ffeFactory >( guiThread );
  factoriesManager.addFactory< CommunicationJrkFactory >( guiThread );
  factoriesManager.addFactory< UbxParserFactory >( calculationsThread );
  factoriesManager.addFactory< NmeaParserGGAFactory >( calculationsThread );
  factoriesManager.addFactory< NmeaParserHDTFactory >( calculationsThread );
  factoriesManager.addFactory< NmeaParserRMCFactory >( calculationsThread );
  factoriesManager.addFactory< AckermannSteeringFactory >( calculationsThread );
  factoriesManager.addFactory< AngularVelocityLimiterFactory >( calculationsThread );

  factoriesManager.addFactory< ValueTransmissionNumberFactory >( calculationsThread );
  factoriesManager.addFactory< ValueTransmissionQuaternionFactory >( calculationsThread );
  factoriesManager.addFactory< ValueTransmissionImuDataFactory >( calculationsThread );
  factoriesManager.addFactory< ValueTransmissionStateFactory >( calculationsThread );
  factoriesManager.addFactory< ValueTransmissionBase64DataFactory >( calculationsThread );

  // simulator block
  auto poseSimulationFactory = std::make_unique< PoseSimulationFactory >(
    calculationsThread, mainWindow, geographicConvertionWrapper.get(), backgroundEntity, usePBR );
  auto poseSimulationBlockId = blocksManager.moveObjectToManager( poseSimulationFactory->createBlock( 0 ) );
  qDebug() << "poseSimulationBlockId" << poseSimulationBlockId;
  auto* poseSimulation = static_cast< PoseSimulation* >( blocksManager.getBlock( poseSimulationBlockId ) );

  poseSimulation->openTINFromString( "/home/christian/Schreibtisch/QtOpenGuidance/terrain/test4.geojson" );

  // camera block
  auto cameraControllerFactory = std::make_unique< CameraControllerFactory >( qt3dThread, rootRootEntity, cameraEntity );
  auto cameraControllerBlockId = blocksManager.moveObjectToManager( cameraControllerFactory->createBlock( 0 ) );
  qDebug() << "cameraControllerBlockId" << cameraControllerBlockId;
  CameraController* cameraController = static_cast< CameraController* >( blocksManager.getBlock( cameraControllerBlockId ) );
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  view->installEventFilter( cameraController );

  // grid block
  auto gridModelFactory = std::make_unique< GridModelFactory >( qt3dThread, middlegroundEntity, cameraEntity );
  auto gridModelBlockId = blocksManager.moveObjectToManager( gridModelFactory->createBlock( 0 ) );
  qDebug() << "gridModelBlockId" << gridModelBlockId;
  GridModel* gridModel = static_cast< GridModel* >( blocksManager.getBlock( gridModelBlockId ) );

  // FPS measuremend block
  auto fpsMeasurementFactory = std::make_unique< FpsMeasurementFactory >( qt3dThread, middlegroundEntity );
  blocksManager.moveObjectToManager( fpsMeasurementFactory->createBlock( 0 ) );

  // guidance
  auto fieldManagerFactory =
    std::make_unique< FieldManagerFactory >( calculationsThread, mainWindow, foregroundEntity, usePBR, geographicConvertionWrapper.get() );
  auto fieldManagerId = blocksManager.moveObjectToManager( fieldManagerFactory->createBlock( 0 ) );
  qDebug() << "fieldManagerId" << fieldManagerId;
  auto* fieldManager = static_cast< FieldManager* >( blocksManager.getBlock( fieldManagerId ) );

  // global planner
  auto globalPlannerFactory = std::make_unique< GlobalPlannerFactory >( calculationsThread,
                                                                        mainWindow,
                                                                        KDDockWidgets::Location_OnRight,
                                                                        guidanceToolbar->menu,
                                                                        geographicConvertionWrapper.get(),
                                                                        middlegroundEntity );
  auto globalPlannerBlockId = blocksManager.moveObjectToManager( globalPlannerFactory->createBlock( 0 ) );
  qDebug() << "globalPlannerBlockId" << globalPlannerBlockId;
  auto* globalPlanner = static_cast< GlobalPlanner* >( blocksManager.getBlock( globalPlannerBlockId ) );

  QObject::connect( settingsDialog, &SettingsDialog::plannerSettingsChanged, globalPlanner, &GlobalPlanner::setPlannerSettings );

  auto* newAbLineAction = newOpenSaveToolbar->newMenu->addAction( QStringLiteral( "New AB-Line/Curve" ) );
  QObject::connect( newAbLineAction, &QAction::triggered, globalPlanner, &GlobalPlanner::newAbLine );

  auto* openAbLineAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open AB-Line/Curve" ) );
  QObject::connect( openAbLineAction, &QAction::triggered, globalPlanner->openSaveHelper, &OpenSaveHelper::open );

  auto* saveAbLineAction = newOpenSaveToolbar->saveMenu->addAction( QStringLiteral( "Save AB-Line/Curve" ) );
  QObject::connect( saveAbLineAction, &QAction::triggered, globalPlanner->openSaveHelper, &OpenSaveHelper::save );

  // SPNAV
#ifdef SPNAV_ENABLED
  qDebug() << "SPNAV_ENABLED";
  auto spaceNavigatorPollingThread = new SpaceNavigatorPollingThread( mainWindow );
  spaceNavigatorPollingThread->start();

  QObject::connect( spaceNavigatorPollingThread,
                    &SpaceNavigatorPollingThread::steerAngleChanged,
                    poseSimulation,
                    &PoseSimulation::setSteerAngle,
                    Qt::QueuedConnection );
  QObject::connect( spaceNavigatorPollingThread,
                    &SpaceNavigatorPollingThread::velocityChanged,
                    poseSimulation,
                    &PoseSimulation::setVelocity,
                    Qt::QueuedConnection );
#endif

// SDL -> controller input
#ifdef SDL2_ENABLED
  qDebug() << "SDL2_ENABLED";
  auto sdlInputPollingThread = new SdlInputPollingThread( mainWindow );
  sdlInputPollingThread->start();

  QObject::connect( sdlInputPollingThread,
                    &SdlInputPollingThread::steerAngleChanged,
                    poseSimulation,
                    &PoseSimulation::setSteerAngle,
                    Qt::QueuedConnection );
  QObject::connect(
    sdlInputPollingThread, &SdlInputPollingThread::velocityChanged, poseSimulation, &PoseSimulation::setVelocity, Qt::QueuedConnection );
#endif

  // Setting Dialog
  QObject::connect( guidanceToolbar, &GuidanceToolbar::toggleSettings, settingsDialog, &SettingsDialog::toggleVisibility );

  // camera dock -> camera controller
  QObject::connect( cameraToolbar, SIGNAL( zoomIn() ), cameraController, SLOT( zoomIn() ) );
  QObject::connect( cameraToolbar, SIGNAL( zoomOut() ), cameraController, SLOT( zoomOut() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltUp() ), cameraController, SLOT( tiltUp() ) );
  QObject::connect( cameraToolbar, SIGNAL( tiltDown() ), cameraController, SLOT( tiltDown() ) );
  QObject::connect( cameraToolbar, SIGNAL( panLeft() ), cameraController, SLOT( panLeft() ) );
  QObject::connect( cameraToolbar, SIGNAL( panRight() ), cameraController, SLOT( panRight() ) );
  QObject::connect( cameraToolbar, SIGNAL( resetCamera() ), cameraController, SLOT( resetCamera() ) );
  QObject::connect( cameraToolbar, SIGNAL( setMode( int ) ), cameraController, SLOT( setMode( int ) ) );
  QObject::connect( settingsDialog, &SettingsDialog::cameraSmoothingChanged, cameraController, &CameraController::setCameraSmoothing );

  // settings dialog -> grid model
  QObject::connect( settingsDialog, SIGNAL( setGrid( bool ) ), gridModel, SLOT( setGrid( bool ) ) );
  QObject::connect( settingsDialog,
                    SIGNAL( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ),
                    gridModel,
                    SLOT( setGridValues( float, float, float, float, float, float, float, QColor, QColor ) ) );

  // Simulator Docks
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorVelocity, &QWidget::setEnabled );
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorSteeringAngle, &QWidget::setEnabled );
  QObject::connect( guidanceToolbar, &GuidanceToolbar::simulatorChanged, simulatorFrequency, &QWidget::setEnabled );

  // connect the signals of the simulator
  {
    QObject::connect( guidanceToolbar, SIGNAL( simulatorChanged( bool ) ), poseSimulation, SLOT( setSimulation( bool ) ) );
    QObject::connect( simulatorVelocity, SIGNAL( valueChanged( double ) ), poseSimulation, SLOT( setVelocity( double ) ) );
    QObject::connect( simulatorSteeringAngle, SIGNAL( valueChanged( double ) ), poseSimulation, SLOT( setSteerAngle( double ) ) );
    QObject::connect( simulatorFrequency, SIGNAL( valueChanged( double ) ), poseSimulation, SLOT( setFrequency( double ) ) );

    QObject::connect( settingsDialog, &SettingsDialog::simulatorValuesChanged, poseSimulation, &PoseSimulation::setSimulatorValues );
    QObject::connect(
      settingsDialog, &SettingsDialog::noiseStandartDeviationsChanged, poseSimulation, &PoseSimulation::setNoiseStandartDeviations );
    QObject::connect( poseSimulation, &PoseSimulation::simulatorValuesChanged, settingsDialog, &SettingsDialog::setSimulatorValues );

    auto* openFieldAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open Terrain Model" ) );
    QObject::connect( openFieldAction, &QAction::triggered, poseSimulation->openSaveHelper, &OpenSaveHelper::open );
  }

  // fieldManager
  {
    auto* newFieldAction = newOpenSaveToolbar->newMenu->addAction( QStringLiteral( "New Field" ) );
    QObject::connect( newFieldAction, &QAction::triggered, fieldManager, &FieldManager::newField );

    auto* openFieldAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open Field" ) );
    QObject::connect( openFieldAction, &QAction::triggered, fieldManager->openSaveHelper, &OpenSaveHelper::open );

    auto* saveFieldAction = newOpenSaveToolbar->saveMenu->addAction( QStringLiteral( "Save Field" ) );
    QObject::connect( saveFieldAction, &QAction::triggered, fieldManager->openSaveHelper, &OpenSaveHelper::save );
  }

  // passes dock -> global planner block
  QObject::connect(
    passesToolbar, SIGNAL( passSettingsChanged( int, int, bool, bool ) ), globalPlanner, SLOT( setPassSettings( int, int, bool, bool ) ) );
  QObject::connect( passesToolbar, SIGNAL( passNumberChanged( int ) ), globalPlanner, SLOT( setPassNumber( int ) ) );

  // field docks -> global planner block
  QObject::connect( fieldsToolbar, SIGNAL( continousRecordToggled( bool ) ), fieldManager, SLOT( setContinousRecord( bool ) ) );
  QObject::connect( fieldsToolbar, SIGNAL( recordPoint() ), fieldManager, SLOT( recordPoint() ) );
  QObject::connect(
    fieldsToolbar, SIGNAL( recordOnEdgeOfImplementChanged( bool ) ), fieldManager, SLOT( recordOnEdgeOfImplementChanged( bool ) ) );
  QObject::connect( fieldsOptimitionToolbar,
                    SIGNAL( recalculateFieldSettingsChanged( FieldsOptimitionToolbar::AlphaType, double, double, double ) ),
                    fieldManager,
                    SLOT( setRecalculateFieldSettings( FieldsOptimitionToolbar::AlphaType, double, double, double ) ) );
  QObject::connect( fieldsOptimitionToolbar, SIGNAL( recalculateField() ), fieldManager, SLOT( recalculateField() ) );
  QObject::connect( fieldManager, SIGNAL( alphaChanged( double, double ) ), fieldsOptimitionToolbar, SLOT( setAlpha( double, double ) ) );

  // ApplicationControlToolbar
  QObject::connect( applicationControlToolbar, &ApplicationControlToolbar::requestClose, mainWindow, &MyMainWindow::close );
  QObject::connect( applicationControlToolbar, &ApplicationControlToolbar::requestFullscreen, mainWindow, &MyMainWindow::toggleFullscreen );
  QObject::connect(
    applicationControlToolbar, &ApplicationControlToolbar::requestSaveConfig, settingsDialog, &SettingsDialog::saveConfigAndDocks );

  // set the defaults for the simulator
  simulatorVelocity->setValue( 0 );
  simulatorSteeringAngle->setValue( 0 );
  simulatorFrequency->setValue( 20 );

  // emit all initial signals from the settings dialog
  settingsDialog->emitAllConfigSignals();

  // load states of checkboxes from global config
  {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

    bool simulatorEnabled = settings.value( QStringLiteral( "RunSimulatorOnStart" ), false ).toBool();
    guidanceToolbar->cbSimulatorSetChecked( simulatorEnabled );
    simulatorVelocity->setEnabled( simulatorEnabled );
    simulatorSteeringAngle->setEnabled( simulatorEnabled );
    simulatorFrequency->setEnabled( simulatorEnabled );

    if( settings.value( QStringLiteral( "OpenSettingsDialogOnStart" ), false ).toBool() ) {
      settingsDialog->show();
    }
  }

  // start all the tasks of settingDialog on start/exit
  settingsDialog->onStart();
  QObject::connect( mainWindow, &MyMainWindow::closed, settingsDialog, &SettingsDialog::onExit );

  mainWindow->layout()->update();
  mainWindow->layout()->activate();
  mainWindow->layout()->update();

  mainWindow->readSettings();

  return QApplication::exec();

  // #ifdef SPNAV_ENABLED
  //   spaceNavigatorPollingThread->stop();

  //  if( !spaceNavigatorPollingThread->wait( 500 ) ) {
  //    spaceNavigatorPollingThread->terminate();
  //  }

  //  spnav_close();
  // #endif
}
