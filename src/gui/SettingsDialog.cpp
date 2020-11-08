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

#include <QMap>

#include <QtWidgets>
#include <QObject>
#include <QSignalBlocker>

#include <QFileDialog>

#ifdef SERIALPORT_ENABLED
#include <QSerialPortInfo>
#include "../block/SerialPort.h"
#endif

#include <QDebug>

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include "MyMainWindow.h"
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QForwardRenderer>

#include "../block/VectorObject.h"
#include "../block/OrientationBlock.h"
#include "../block/NumberObject.h"
#include "../block/StringObject.h"

#include "../block/Implement.h"
#include "../block/PathPlannerModel.h"

#include "../block/CameraController.h"
#include "../block/TractorModel.h"
#include "../block/TrailerModel.h"
#include "../block/SprayerModel.h"
#include "../block/CultivatedAreaModel.h"
#include "../block/GridModel.h"

#include "../block/AckermannSteering.h"
#include "../block/AngularVelocityLimiter.h"

#include "../block/PoseSimulation.h"

#ifdef SPNAV_ENABLED
#include "SpaceNavigatorPollingThread.h"
#endif

#include "../block/ExtendedKalmanFilter.h"
#include "../block/PoseSynchroniser.h"

#include "../block/UbxParser.h"
#include "../block/NmeaParserGGA.h"
#include "../block/NmeaParserHDT.h"
#include "../block/NmeaParserRMC.h"
#include "../block/TransverseMercatorConverter.h"

#include "../block/FieldManager.h"
#include "../block/ActionDockBlock.h"
#include "../block/GlobalPlanner.h"
#include "../block/LocalPlanner.h"
#include "../block/StanleyGuidance.h"
#include "../block/XteGuidance.h"
#include "../block/PathPlannerModel.h"
#include "../block/SectionControl.h"

#include "../block/DebugSink.h"

#include "../block/UdpSocket.h"
#include "../block/FileStream.h"
#include "../block/CommunicationPgn7FFE.h"
#include "../block/CommunicationJrk.h"

#include "../block/ValueTransmissionNumber.h"
#include "../block/ValueTransmissionQuaternion.h"
#include "../block/ValueTransmissionBase64Data.h"
#include "../block/ValueTransmissionState.h"

#include "../kinematic/GeographicConvertionWrapper.h"
#include "../kinematic/FixedKinematic.h"
#include "../kinematic/TrailerKinematic.h"
#include "../kinematic/FixedKinematicPrimitive.h"
#include "../kinematic/TrailerKinematicPrimitive.h"

#include "../kinematic/cgalKernel.h"

#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

SettingsDialog::SettingsDialog( Qt3DCore::QEntity* rootEntity, MyMainWindow* mainWindow, Qt3DExtras::Qt3DWindow* qt3dWindow, QMenu* guidanceToolbarMenu, QWidget* parent ) :
  QDialog( parent ),
  mainWindow( mainWindow ),
  qt3dWindow( qt3dWindow ),
  ui( new Ui::SettingsDialog ) {
  // initialise the wrapper for the geographic conversion, so all offsets are the same application-wide
  geographicConvertionWrapperGuidance = new GeographicConvertionWrapper();
  geographicConvertionWrapperSimulator = new GeographicConvertionWrapper();

  ui->setupUi( this );

  // load states of checkboxes from global config
  {
    blockSettingsSaving = true;

    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    // checkboxes
    {
      ui->cbSaveConfigOnExit->setCheckState( settings.value( QStringLiteral( "SaveConfigOnExit" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbLoadConfigOnStart->setCheckState( settings.value( QStringLiteral( "LoadConfigOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbOpenSettingsDialogOnStart->setCheckState( settings.value( QStringLiteral( "OpenSettingsDialogOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbRunSimulatorOnStart->setCheckState( settings.value( QStringLiteral( "RunSimulatorOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbRestoreDockPositions->setCheckState( settings.value( QStringLiteral( "RestoreDockPositionsOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbSaveDockPositionsOnExit->setCheckState( settings.value( QStringLiteral( "SaveDockPositionsOnExit" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
    }

    // grid
    {
      ui->gbGrid->setChecked( settings.value( QStringLiteral( "Grid/Enabled" ), true ).toBool() );
      ui->dsbGridXStep->setValue( settings.value( QStringLiteral( "Grid/XStep" ), 1 ).toDouble() );
      ui->dsbGridYStep->setValue( settings.value( QStringLiteral( "Grid/YStep" ), 1 ).toDouble() );
      ui->dsbGridXStepCoarse->setValue( settings.value( QStringLiteral( "Grid/XStepCoarse" ), 10 ).toDouble() );
      ui->dsbGridYStepCoarse->setValue( settings.value( QStringLiteral( "Grid/YStepCoarse" ), 10 ).toDouble() );
      ui->dsbGridSize->setValue( settings.value( QStringLiteral( "Grid/Size" ), 10 ).toDouble() );
      ui->dsbGridCameraThreshold->setValue( settings.value( QStringLiteral( "Grid/CameraThreshold" ), 75 ).toDouble() );
      ui->dsbGridCameraThresholdCoarse->setValue( settings.value( QStringLiteral( "Grid/CameraThresholdCoarse" ), 250 ).toDouble() );
      gridColor = settings.value( QStringLiteral( "Grid/Color" ), QColor( 0x6b, 0x96, 0xa8 ) ).value<QColor>();
      gridColorCoarse = settings.value( QStringLiteral( "Grid/ColorCoarse" ), QColor( 0xa2, 0xe3, 0xff ) ).value<QColor>();
    }

    // path planner
    {
      ui->sbPathsInReserve->setValue( settings.value( QStringLiteral( "PathPlanner/PathsInReserve" ), 3 ).toInt() );
      ui->sbGlobalPlannerMaxDeviation->setValue( settings.value( QStringLiteral( "PathPlanner/MaxDeviation" ), 0.1 ).toDouble() );
    }

    // material
    {
      bool usePBR = settings.value( QStringLiteral( "Material/usePBR" ), true ).toBool();
      bool usePhong = settings.value( QStringLiteral( "Material/usePhong" ), true ).toBool();

      if( usePBR && usePhong ) {
        ui->rbMaterialAuto->setChecked( true );
      } else {
        ui->rbMaterialPRB->setChecked( usePBR );
        ui->rbMaterialPhong->setChecked( usePhong );
      }
    }

    // Qt3D
    {
      double gamma = settings.value( QStringLiteral( "Qt3D/Gamma" ), 2.2 ).toDouble();
      bool showDebugOverlay = settings.value( QStringLiteral( "Qt3D/ShowDebugOverlay" ), false ).toBool();

      ui->dsbGamma->setValue( gamma );
      ui->cbShowDebugOverlay->setChecked( showDebugOverlay );
    }

    // camera smoothing
    {
      ui->slCameraSmoothingOrientation->setValue( settings.value( QStringLiteral( "Camera/orientationSmoothing" ), 0.9 ).toFloat() * 100 );
      ui->slCameraSmoothingPosition->setValue( settings.value( QStringLiteral( "Camera/positionSmoothing" ), 0.5 ).toFloat() * 100 );
    }

    blockSettingsSaving = false;
  }

  bool usePBR = true;

  if( qt3dWindow->format().majorVersion() <= 2 || ui->rbMaterialPhong->isChecked() ) {
    usePBR = false;
  }

  ui->gvNodeEditor->setDragMode( QGraphicsView::RubberBandDrag );

  auto* scene = new QGraphicsScene();
  ui->gvNodeEditor->setScene( scene );

  ui->gvNodeEditor->setRenderHint( QPainter::Antialiasing, true );

  auto* nodesEditor = new QNodesEditor( this );
  nodesEditor->install( scene );

  // new/open/save toolbar
  newOpenSaveToolbar = new NewOpenSaveToolbar( this );
  auto* newOpenSaveToolbarDock = new KDDockWidgets::DockWidget( QStringLiteral( "NewOpenSaveToolbarDock" ) );
  newOpenSaveToolbarDock->setWidget( newOpenSaveToolbar );
  newOpenSaveToolbarDock->setTitle( "New, Save, Open" );
  mainWindow->addDockWidget( newOpenSaveToolbarDock, KDDockWidgets::Location_OnLeft );
  guidanceToolbarMenu->addAction( newOpenSaveToolbarDock->toggleAction() );

  // Models for the tableview
  filterModelValues = new QSortFilterProxyModel( scene );
  vectorBlockModel = new VectorBlockModel( scene );
  orientationBlockModel = new OrientationBlockModel( scene );
  numberBlockModel = new NumberBlockModel( scene );
  actionBlockModel = new ActionDockBlockModel( scene );
  sliderBlockModel = new SliderDockBlockModel( scene );
  stringBlockModel = new StringBlockModel( scene );

  vectorBlockModel->addToCombobox( ui->cbValues );
  orientationBlockModel->addToCombobox( ui->cbValues );
  numberBlockModel->addToCombobox( ui->cbValues );
  actionBlockModel->addToCombobox( ui->cbValues );
  sliderBlockModel->addToCombobox( ui->cbValues );
  stringBlockModel->addToCombobox( ui->cbValues );
  filterModelValues->setSourceModel( vectorBlockModel );
  filterModelValues->sort( 0, Qt::AscendingOrder );
  ui->twValues->setModel( filterModelValues );

  implementBlockModel = new ImplementBlockModel( scene );
  filterModelImplements = new QSortFilterProxyModel( scene );
  filterModelImplements->setDynamicSortFilter( true );
  filterModelImplements->setSourceModel( implementBlockModel );
  filterModelImplements->sort( 0, Qt::AscendingOrder );
  ui->cbImplements->setModel( filterModelImplements );
  ui->cbImplements->setCurrentIndex( 0 );
  QObject::connect( implementBlockModel, &QAbstractItemModel::modelReset,
                    this, &SettingsDialog::implementModelReset );

  implementSectionModel = new ImplementSectionModel();
  ui->twSections->setModel( implementSectionModel );

  meterModel = new ValueBlockModel( scene );
  filterModelMeter = new QSortFilterProxyModel( scene );
  filterModelMeter->setDynamicSortFilter( true );
  filterModelMeter->setSourceModel( meterModel );
  filterModelMeter->sort( 0, Qt::AscendingOrder );
  ui->tvMeter->setModel( filterModelMeter );
  meterModelFontDelegate = new FontComboboxDelegate( ui->tvMeter );
  ui->tvMeter->setItemDelegateForColumn( 6, meterModelFontDelegate );

  plotBlockModel = new PlotBlockModel( scene );
  filterModelPlot = new QSortFilterProxyModel( scene );
  filterModelPlot->setDynamicSortFilter( true );
  filterModelPlot->setSourceModel( plotBlockModel );
  filterModelPlot->sort( 0, Qt::AscendingOrder );
  ui->tvPlots->setModel( filterModelPlot );

  transmissionBlockModel = new TransmissionBlockModel( scene );
  filterTtransmissionBlockModel = new QSortFilterProxyModel( scene );
  filterTtransmissionBlockModel->setDynamicSortFilter( true );
  filterTtransmissionBlockModel->setSourceModel( transmissionBlockModel );
  filterTtransmissionBlockModel->sort( 0, Qt::AscendingOrder );
  ui->tvTransmission->setModel( filterTtransmissionBlockModel );

  pathPlannerModelBlockModel = new PathPlannerModelBlockModel( scene );
  filterModelPathPlannerModel = new QSortFilterProxyModel( scene );
  filterModelPathPlannerModel->setDynamicSortFilter( true );
  filterModelPathPlannerModel->setSourceModel( pathPlannerModelBlockModel );
  filterModelPathPlannerModel->sort( 0, Qt::AscendingOrder );
  ui->cbPathPlanner->setModel( filterModelPathPlannerModel );
  ui->cbPathPlanner->setCurrentIndex( 0 );
  QObject::connect( pathPlannerModelBlockModel, &QAbstractItemModel::modelReset,
                    this, &SettingsDialog::pathPlannerModelReset );

  // simulator
  poseSimulationFactory = new PoseSimulationFactory( geographicConvertionWrapperSimulator );
  auto* poseSimulationBlock = poseSimulationFactory->createBlock( ui->gvNodeEditor->scene() );
  poseSimulation = qobject_cast<PoseSimulation*>( poseSimulationBlock->object );

  auto poseSimulationTmp = qobject_cast<PoseSimulation*>( poseSimulationBlock->object );;
  QObject::connect( this, &SettingsDialog::noiseStandartDeviationsChanged,
                    poseSimulationTmp, &PoseSimulation::setNoiseStandartDeviations );

  // SPNAV
#ifdef SPNAV_ENABLED
  qDebug() << "SPNAV_ENABLED";
  spaceNavigatorPollingThread = new SpaceNavigatorPollingThread( this );
  spaceNavigatorPollingThread->start();

  connect( spaceNavigatorPollingThread, &SpaceNavigatorPollingThread::steerAngleChanged,
           poseSimulationTmp, &PoseSimulation::setSteerAngle, Qt::QueuedConnection );
  connect( spaceNavigatorPollingThread, &SpaceNavigatorPollingThread::velocityChanged,
           poseSimulationTmp, &PoseSimulation::setVelocity, Qt::QueuedConnection );
#endif

  // guidance
  fieldManagerFactory = new FieldManagerFactory( mainWindow, rootEntity, geographicConvertionWrapperGuidance );
  auto* fieldManagerBlock = fieldManagerFactory->createBlock( ui->gvNodeEditor->scene() );

  {
    auto fieldManagerObject = qobject_cast<FieldManager*>( fieldManagerBlock->object );
    auto newFieldAction = newOpenSaveToolbar->newMenu->addAction( QStringLiteral( "New Field" ) );
    QObject::connect( newFieldAction, &QAction::triggered, fieldManagerObject, &FieldManager::newField );

    auto openFieldAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open Field" ) );
    QObject::connect( openFieldAction, &QAction::triggered, fieldManagerObject, &FieldManager::openField );

    auto saveFieldAction = newOpenSaveToolbar->saveMenu->addAction( QStringLiteral( "Save Field" ) );
    QObject::connect( saveFieldAction, &QAction::triggered, fieldManagerObject, &FieldManager::saveField );

    fieldManager = fieldManagerObject;
  }

  globalPlannerFactory = new GlobalPlannerFactory( mainWindow,
      KDDockWidgets::Location_OnRight,
      guidanceToolbarMenu,
      geographicConvertionWrapperGuidance,
      rootEntity );
  auto* globalPlannerBlock = globalPlannerFactory->createBlock( ui->gvNodeEditor->scene() );

  {
    auto globalPlanner = qobject_cast<GlobalPlanner*>( globalPlannerBlock->object );

    QObject::connect( this, &SettingsDialog::plannerSettingsChanged, globalPlanner, &GlobalPlanner::setPlannerSettings );

    auto newFieldAction = newOpenSaveToolbar->newMenu->addAction( QStringLiteral( "New AB-Line/Curve" ) );
    QObject::connect( newFieldAction, &QAction::triggered, globalPlanner, &GlobalPlanner::newField );

    auto openFieldAction = newOpenSaveToolbar->openMenu->addAction( QStringLiteral( "Open AB-Line/Curve" ) );
    QObject::connect( openFieldAction, &QAction::triggered, globalPlanner, &GlobalPlanner::openAbLine );

    auto saveFieldAction = newOpenSaveToolbar->saveMenu->addAction( QStringLiteral( "Save AB-Line/Curve" ) );
    QObject::connect( saveFieldAction, &QAction::triggered, globalPlanner, &GlobalPlanner::saveAbLine );

    this->globalPlanner = globalPlanner;
  }

  localPlannerFactory = new LocalPlannerFactory( mainWindow,
      KDDockWidgets::Location_OnRight,
      guidanceToolbarMenu );
  stanleyGuidanceFactory = new StanleyGuidanceFactory();
  xteGuidanceFactory = new XteGuidanceFactory();
  sectionControlFactory = new SectionControlFactory( mainWindow,
      KDDockWidgets::Location_OnBottom,
      guidanceToolbarMenu,
      rootEntity,
      static_cast<Qt3DRender::QFrameGraphNode*>( qt3dWindow->activeFrameGraph()->children().front() ) );

  pathPlannerModelFactory = new PathPlannerModelFactory( rootEntity );

  // Factories for the blocks
  transverseMercatorConverterFactory = new TransverseMercatorConverterFactory( geographicConvertionWrapperGuidance );
  poseSynchroniserFactory = new PoseSynchroniserFactory();
  extendedKalmanFilterFactory = new ExtendedKalmanFilterFactory();
  trailerModelFactory = new TrailerModelFactory( rootEntity, usePBR );
  tractorModelFactory = new TractorModelFactory( rootEntity, usePBR );
  sprayerModelFactory = new SprayerModelFactory( rootEntity, usePBR );
  cultivatedAreaModelFactory = new CultivatedAreaModelFactory( rootEntity, usePBR );
  fixedKinematicFactory = new FixedKinematicFactory;
  trailerKinematicFactory = new TrailerKinematicFactory();
  fixedKinematicPrimitiveFactory = new FixedKinematicPrimitiveFactory;
  trailerKinematicPrimitiveFactory = new TrailerKinematicPrimitiveFactory();
  vectorFactory = new VectorFactory( vectorBlockModel );
  orientationFactory = new OrientationBlockFactory( orientationBlockModel );
  numberFactory = new NumberFactory( numberBlockModel );
  stringFactory = new StringFactory( stringBlockModel );
  debugSinkFactory = new DebugSinkFactory();
  udpSocketFactory = new UdpSocketFactory();

#ifdef SERIALPORT_ENABLED
  serialPortFactory = new SerialPortFactory();
#endif

  fileStreamFactory = new FileStreamFactory();
  communicationPgn7ffeFactory = new CommunicationPgn7ffeFactory();
  communicationJrkFactory = new CommunicationJrkFactory();
  ubxParserFactory = new UbxParserFactory();
  nmeaParserGGAFactory = new NmeaParserGGAFactory();
  nmeaParserHDTFactory = new NmeaParserHDTFactory();
  nmeaParserRMCFactory = new NmeaParserRMCFactory();
  ackermannSteeringFactory = new AckermannSteeringFactory();
  angularVelocityLimiterFactory = new AngularVelocityLimiterFactory();

  valueTransmissionNumberFactory = new ValueTransmissionNumberFactory();
  valueTransmissionQuaternionFactory = new ValueTransmissionQuaternionFactory();
  valueTransmissionStateFactory = new ValueTransmissionStateFactory();
  valueTransmissionBase64DataFactory = new ValueTransmissionBase64DataFactory();

  vectorFactory->addToTreeWidget( ui->twBlocks );
  orientationFactory->addToTreeWidget( ui->twBlocks );
  numberFactory->addToTreeWidget( ui->twBlocks );
  stringFactory->addToTreeWidget( ui->twBlocks );
  fixedKinematicFactory->addToTreeWidget( ui->twBlocks );
  fixedKinematicPrimitiveFactory->addToTreeWidget( ui->twBlocks );
  tractorModelFactory->addToTreeWidget( ui->twBlocks );
  trailerKinematicFactory->addToTreeWidget( ui->twBlocks );
  trailerKinematicPrimitiveFactory->addToTreeWidget( ui->twBlocks );
  trailerModelFactory->addToTreeWidget( ui->twBlocks );
  sprayerModelFactory->addToTreeWidget( ui->twBlocks );
  cultivatedAreaModelFactory->addToTreeWidget( ui->twBlocks );
  ackermannSteeringFactory->addToTreeWidget( ui->twBlocks );
  angularVelocityLimiterFactory->addToTreeWidget( ui->twBlocks );
  poseSynchroniserFactory->addToTreeWidget( ui->twBlocks );
  extendedKalmanFilterFactory->addToTreeWidget( ui->twBlocks );
  transverseMercatorConverterFactory->addToTreeWidget( ui->twBlocks );
  xteGuidanceFactory->addToTreeWidget( ui->twBlocks );
  stanleyGuidanceFactory->addToTreeWidget( ui->twBlocks );
  localPlannerFactory->addToTreeWidget( ui->twBlocks );
  sectionControlFactory->addToTreeWidget( ui->twBlocks );
  pathPlannerModelFactory->addToTreeWidget( ui->twBlocks );
  ubxParserFactory->addToTreeWidget( ui->twBlocks );
  nmeaParserGGAFactory->addToTreeWidget( ui->twBlocks );
  nmeaParserHDTFactory->addToTreeWidget( ui->twBlocks );
  nmeaParserRMCFactory->addToTreeWidget( ui->twBlocks );
  debugSinkFactory->addToTreeWidget( ui->twBlocks );

  valueTransmissionNumberFactory->addToTreeWidget( ui->twBlocks );
  valueTransmissionQuaternionFactory->addToTreeWidget( ui->twBlocks );
  valueTransmissionStateFactory->addToTreeWidget( ui->twBlocks );
  valueTransmissionBase64DataFactory->addToTreeWidget( ui->twBlocks );

  udpSocketFactory->addToTreeWidget( ui->twBlocks );

#ifdef SERIALPORT_ENABLED
  serialPortFactory->addToTreeWidget( ui->twBlocks );
#endif

  fileStreamFactory->addToTreeWidget( ui->twBlocks );
  communicationPgn7ffeFactory->addToTreeWidget( ui->twBlocks );
  communicationJrkFactory->addToTreeWidget( ui->twBlocks );

  // grid color picker
  ui->lbColor->setText( gridColor.name() );
  ui->lbColor->setPalette( QPalette( gridColor ) );
  ui->lbColor->setAutoFillBackground( true );
  ui->lbColorCoarse->setText( gridColorCoarse.name() );
  ui->lbColorCoarse->setPalette( QPalette( gridColorCoarse ) );
  ui->lbColorCoarse->setAutoFillBackground( true );

  this->on_pbBaudrateRefresh_clicked();
  this->on_pbComPortRefresh_clicked();

  // draw an axis-cross: X-red, Y-green, Z-blue
  if( true ) {
    constexpr float metalness = 0.1f;
    constexpr float roughness = 0.5f;


    auto* xAxis = new Qt3DCore::QEntity( rootEntity );
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
      auto* yAxis = new Qt3DCore::QEntity( rootEntity );

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
      auto* zAxis = new Qt3DCore::QEntity( rootEntity );

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
}

SettingsDialog::~SettingsDialog() {
  delete ui;

  transverseMercatorConverterFactory->deleteLater();
  poseSynchroniserFactory->deleteLater();
  extendedKalmanFilterFactory->deleteLater();
  tractorModelFactory->deleteLater();
  trailerModelFactory->deleteLater();
  sprayerModelFactory->deleteLater();
  fixedKinematicFactory->deleteLater();
  trailerKinematicFactory->deleteLater();
  fixedKinematicPrimitiveFactory->deleteLater();
  trailerKinematicPrimitiveFactory->deleteLater();
  vectorFactory->deleteLater();
  numberFactory->deleteLater();
  stringFactory->deleteLater();
  debugSinkFactory->deleteLater();
  udpSocketFactory->deleteLater();

  valueTransmissionNumberFactory->deleteLater();
  valueTransmissionQuaternionFactory->deleteLater();
  valueTransmissionStateFactory->deleteLater();
  valueTransmissionBase64DataFactory->deleteLater();

#ifdef SERIALPORT_ENABLED
  serialPortFactory->deleteLater();
#endif

  fileStreamFactory->deleteLater();
  communicationPgn7ffeFactory->deleteLater();
  communicationJrkFactory->deleteLater();
  nmeaParserGGAFactory->deleteLater();
  nmeaParserHDTFactory->deleteLater();
  nmeaParserRMCFactory->deleteLater();
  ackermannSteeringFactory->deleteLater();

  vectorBlockModel->deleteLater();
  numberBlockModel->deleteLater();
  stringBlockModel->deleteLater();
  sliderBlockModel->deleteLater();
  actionBlockModel->deleteLater();

  implementBlockModel->deleteLater();
  implementSectionModel->deleteLater();

  poseSimulationFactory->deleteLater();
  poseSimulation->deleteLater();

#ifdef SPNAV_ENABLED
  spaceNavigatorPollingThread->stop();

  if( ! spaceNavigatorPollingThread->wait( 500 ) ) {
    spaceNavigatorPollingThread->terminate();
  }

  spnav_close();
#endif

  plannerGuiFactory->deleteLater();
  plannerGui->deleteLater();
  globalPlannerFactory->deleteLater();
  globalPlanner->deleteLater();
  localPlannerFactory->deleteLater();
  localPlanner->deleteLater();
  stanleyGuidanceFactory->deleteLater();
  stanleyGuidance->deleteLater();
  xteGuidanceFactory->deleteLater();
  xteGuidance->deleteLater();
  pathPlannerModelFactory->deleteLater();
  globalPlannerModel->deleteLater();
}

QGraphicsScene* SettingsDialog::getSceneOfConfigGraphicsView() {
  return ui->gvNodeEditor->scene();
}

void SettingsDialog::toggleVisibility() {
  if( isVisible() ) {
    setVisible( false );
  } else {
#ifdef Q_OS_ANDROID
    showMaximized();
#else
    show();
#endif
  }
}

void SettingsDialog::onStart() {
  // save the current config if enabled
  if( ui->cbLoadConfigOnStart->isChecked() ) {
    loadDefaultConfig();
  }

  if( ui->cbRestoreDockPositions->isChecked() ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );
    KDDockWidgets::LayoutSaver saver;

    saver.restoreLayout( settings.value( QStringLiteral( "SavedDocks" ) ).toByteArray() );
  }

  ui->twBlocks->setSortingEnabled( true );
  ui->twBlocks->sortByColumn( 0, Qt::AscendingOrder );
}

void SettingsDialog::onExit() {
  // save the current config if enabled
  if( ui->cbSaveConfigOnExit->isChecked() ) {
    saveDefaultConfig();
  }

  if( ui->cbSaveDockPositionsOnExit->isChecked() ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );
    KDDockWidgets::LayoutSaver saver;

    settings.setValue( QStringLiteral( "SavedDocks" ), saver.serializeLayout() );
    settings.sync();
  }
}

void SettingsDialog::loadDefaultConfig() {
  QFile saveFile( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/default.json" );

  if( saveFile.open( QIODevice::ReadOnly ) ) {
    loadConfigFromFile( saveFile );
  }

  ui->gvNodeEditor->fitInView( ui->gvNodeEditor->scene()->itemsBoundingRect(), Qt::KeepAspectRatio );

  ui->gvNodeEditor->scene()->clearSelection();
}

void SettingsDialog::saveDefaultConfig() {
  QFile saveFile( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/default.json" );

  if( saveFile.open( QIODevice::WriteOnly ) ) {

    // select all items, so everything gets saved
    {
      const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

      for( const auto& item : constRefOfList ) {
        item->setSelected( true );
      }
    }

    saveConfigToFile( saveFile );

    // deselect all items
    {
      const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

      for( const auto& item : constRefOfList ) {
        item->setSelected( false );
      }
    }
  }
}

void SettingsDialog::on_cbValues_currentIndexChanged( int /*index*/ ) {
  auto* model = qobject_cast<QAbstractTableModel*>( qvariant_cast<QAbstractTableModel*>( ui->cbValues->currentData() ) );

  if( model != nullptr ) {
    filterModelValues->setSourceModel( model );
    ui->twValues->resizeColumnsToContents();
  }

  ui->grpString->setHidden( !( ui->cbValues->currentText() == QLatin1String( "String" ) ) );
  ui->grpNumber->setHidden( !( ui->cbValues->currentText() == QLatin1String( "Number" ) ) );
}

void SettingsDialog::on_pbSaveSelected_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json)" );
  QString dir;
  QString fileName = QFileDialog::getSaveFileName( this,
                     tr( "Open Saved Config" ),
                     dir,
                     tr( "All Files (*);;JSON Files (*.json)" ),
                     &selectedFilter );

  if( !fileName.isEmpty() ) {

    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning() << "Couldn't open save file.";
      return;
    }

    saveConfigToFile( saveFile );
  }
}

void SettingsDialog::saveConfigToFile( QFile& file ) {
  QJsonObject jsonObject;
  QJsonArray blocks;
  QJsonArray connections;
  jsonObject[QStringLiteral( "blocks" )] = blocks;
  jsonObject[QStringLiteral( "connections" )] = connections;

  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    {
      auto* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block != nullptr ) {
        block->toJSON( jsonObject );
      }
    }

    {
      auto* connection = qgraphicsitem_cast<QNEConnection*>( item );

      if( connection != nullptr ) {
        connection->toJSON( jsonObject );
      }
    }
  }

  QJsonDocument jsonDocument( jsonObject );
  file.write( jsonDocument.toJson() );
}

QNEBlock* SettingsDialog::getBlockWithId( int id ) {
  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( block->id == id ) {
        return block;
      }
    }
  }

  return nullptr;
}

QNEBlock* SettingsDialog::getBlockWithName( const QString& name ) {
  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( block->getName() == name ) {
        return block;
      }
    }
  }

  return nullptr;
}

void SettingsDialog::on_pbLoad_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json)" );
  QString dir;

  auto* fileDialog = new QFileDialog( this,
                                      tr( "Open Saved Config" ),
                                      dir,
                                      selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;JSON Files (*.json)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect to
  // the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    qDebug() << "QFileDialog::urlSelected QUrl" << fileName << fileName.toDisplayString() << fileName.toLocalFile();

    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile(
        QUrl::fromPercentEncoding(
          fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {

        loadConfigFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#else
  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, [this, fileDialog]( const QString & fileName ) {
    qDebug() << "QFileDialog::fileSelected QString" << fileName;

    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {

        loadConfigFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#endif

  // connect finished to deleteLater, so the dialog gets deleted when Cancel is pressed
  QObject::connect( fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater );

  fileDialog->open();
}

void SettingsDialog::loadConfigFromFile( QFile& file ) {
  QByteArray saveData = file.readAll();

  QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
  QJsonObject json = loadDoc.object();

  // as the new object get new id, here is a QMap to hold the conversions
  // first int: id in file, second int: id in the graphicsview
  QMap<int, int> idMap;

  if( json.contains( QStringLiteral( "blocks" ) ) && json[QStringLiteral( "blocks" )].isArray() ) {
    QJsonArray blocksArray = json[QStringLiteral( "blocks" )].toArray();

    for( const auto& blockIndex : qAsConst( blocksArray ) ) {
      QJsonObject blockObject = blockIndex.toObject();
      int id = blockObject[QStringLiteral( "id" )].toInt( 0 );

      // if id is a system-id -> search the block and set the values
      if( id != 0 ) {

        // system id -> don't create new blocks
        if( id < int( QNEBlock::IdRange::UserIdStart ) ) {
          QNEBlock* block = getBlockWithName( blockObject[QStringLiteral( "type" )].toString() );

          if( block != nullptr ) {
            idMap.insert( id, block->id );
            block->setX( blockObject[QStringLiteral( "positionX" )].toDouble( 0 ) );
            block->setY( blockObject[QStringLiteral( "positionY" )].toDouble( 0 ) );
          }

          // id is not a system-id -> create new blocks
        } else {
          BlockFactory* factory = nullptr;

          {
            auto rootItem = ui->twBlocks->invisibleRootItem();

            for( int i = 0, end = rootItem->childCount(); i < end; i++ ) {
              auto item = rootItem->child( i );

              for( int j = 0, end = item->childCount(); j < end; j++ ) {
                auto childItem = item->child( j );

                if( childItem->text( 1 ) == blockObject[QStringLiteral( "type" )].toString() ) {
                  factory = qobject_cast<BlockFactory*>( qvariant_cast<QObject*>( childItem->data( 0, Qt::UserRole ) ) );
                }
              }
            }
          }

          if( factory != nullptr ) {
            QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene(), id );

            idMap.insert( id, block->id );

            block->setX( blockObject[QStringLiteral( "positionX" )].toDouble( 0 ) );
            block->setY( blockObject[QStringLiteral( "positionY" )].toDouble( 0 ) );
            block->setName( blockObject[QStringLiteral( "name" )].toString( factory->getNameOfFactory() ) );
            block->fromJSON( blockObject );
            block->setSelected( true );
          }
        }
      }
    }
  }


  if( json.contains( QStringLiteral( "connections" ) ) && json[QStringLiteral( "connections" )].isArray() ) {
    QJsonArray connectionsArray = json[QStringLiteral( "connections" )].toArray();

    for( const auto& connectionsIndex : qAsConst( connectionsArray ) ) {
      QJsonObject connectionsObject = connectionsIndex.toObject();

      if( !connectionsObject[QStringLiteral( "idFrom" )].isUndefined() &&
          !connectionsObject[QStringLiteral( "idTo" )].isUndefined() &&
          !connectionsObject[QStringLiteral( "portFrom" )].isUndefined() &&
          !connectionsObject[QStringLiteral( "portTo" )].isUndefined() ) {

        int idFrom = idMap[connectionsObject[QStringLiteral( "idFrom" )].toInt()];
        int idTo = idMap[connectionsObject[QStringLiteral( "idTo" )].toInt()];

        if( idFrom != 0 && idTo != 0 ) {
          QNEBlock* blockFrom = getBlockWithId( idFrom );
          QNEBlock* blockTo = getBlockWithId( idTo );

          if( ( blockFrom != nullptr ) && ( blockTo != nullptr ) ) {
            QString portFromName = connectionsObject[QStringLiteral( "portFrom" )].toString();
            QString portToName = connectionsObject[QStringLiteral( "portTo" )].toString();

            QNEPort* portFrom = blockFrom->getPortWithName( portFromName, true );
            QNEPort* portTo = blockTo->getPortWithName( portToName, false );

            if( ( portFrom != nullptr ) && ( portTo != nullptr ) ) {
              auto* conn = new QNEConnection();
              conn->setPort1( portFrom );

              if( conn->setPort2( portTo ) ) {
                blockFrom->scene()->addItem( conn );
                conn->updatePosFromPorts();
                conn->updatePath();
                conn->setSelected( true );
              } else {
                delete conn;
              }
            }
          }
        }
      }
    }
  }

  // as new values for the blocks are added above, emit all signals now, when the connections are made
  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      block->emitConfigSignals();
    }
  }

  allModelsReset();
}

void SettingsDialog::on_pbAddBlock_clicked() {
  auto results = ui->twBlocks->selectedItems();

  if( results.size() >= 1 ) {
    auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<QObject*>( results.first()->data( 0, Qt::UserRole ) ) );

    if( factory != nullptr ) {
      QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene() );
      block->setPos( ui->gvNodeEditor->mapToScene( ui->gvNodeEditor->viewport()->rect().center() ) );
    }

    allModelsReset();
  }
}

void SettingsDialog::on_pbZoomOut_clicked() {
  ui->gvNodeEditor->zoomOut();
}

void SettingsDialog::on_pbZoomIn_clicked() {
  ui->gvNodeEditor->zoomIn();
}

void SettingsDialog::on_pbDeleteSelected_clicked() {
  {
    const auto& constRefOfList = ui->gvNodeEditor->scene()->selectedItems();

    for( int i = 0, end = constRefOfList.size(); i < end; ++i ) {
      auto* connection = qgraphicsitem_cast<QNEConnection*>( constRefOfList.at( i ) );
      delete connection;
    }
  }

  {
    const auto& constRefOfList = ui->gvNodeEditor->scene()->selectedItems();

    for( int i = 0, end = constRefOfList.size(); i < end; ++i ) {
      auto* block = qgraphicsitem_cast<const QNEBlock*>( constRefOfList.at( i ) );

      if( block != nullptr ) {
        if( !block->systemBlock ) {
          delete block;
        }
      }
    }
  }

  allModelsReset();

}

void SettingsDialog:: on_gbGrid_toggled( bool arg1 ) {
  saveGridValuesInSettings();
  emit setGrid( bool( arg1 ) );
}

void SettingsDialog::on_dsbGridXStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( double( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::on_dsbGridYStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( double( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::on_dsbGridSize_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( double( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::on_pbColor_clicked() {
  const QColor color = QColorDialog::getColor( gridColor, this, QStringLiteral( "Select Grid Color" ) );

  if( color.isValid() ) {
    gridColor = color;
    ui->lbColor->setText( gridColor.name() );
    ui->lbColor->setPalette( QPalette( gridColor ) );
    ui->lbColor->setAutoFillBackground( true );

    saveGridValuesInSettings();
    emit setGridValues( double( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                        float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                        float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                        gridColor, gridColorCoarse );
  }
}

void SettingsDialog::saveGridValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( QStringLiteral( "Grid/Enabled" ), bool( ui->gbGrid->isChecked() ) );
    settings.setValue( QStringLiteral( "Grid/XStep" ), ui->dsbGridXStep->value() );
    settings.setValue( QStringLiteral( "Grid/YStep" ), ui->dsbGridYStep->value() );
    settings.setValue( QStringLiteral( "Grid/XStepCoarse" ), ui->dsbGridXStepCoarse->value() );
    settings.setValue( QStringLiteral( "Grid/YStepCoarse" ), ui->dsbGridYStepCoarse->value() );
    settings.setValue( QStringLiteral( "Grid/Size" ), ui->dsbGridSize->value() );
    settings.setValue( QStringLiteral( "Grid/CameraThreshold" ), ui->dsbGridCameraThreshold->value() );
    settings.setValue( QStringLiteral( "Grid/CameraThresholdCoarse" ), ui->dsbGridCameraThresholdCoarse->value() );
    settings.setValue( QStringLiteral( "Grid/Color" ), gridColor );
    settings.sync();
  }
}

void SettingsDialog::savePathPlannerValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( QStringLiteral( "PathPlanner/PathsInReserve" ), ui->sbPathsInReserve->value() );
    settings.setValue( QStringLiteral( "PathPlanner/MaxDeviation" ), ui->sbGlobalPlannerMaxDeviation->value() );
    settings.sync();
  }
}

void SettingsDialog::setPathPlannerSettings() {
  QModelIndex idx = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {

    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {
      pathPlannerModel->visible = ui->gbPathPlanner->isChecked();
      block->setName( ui->lePathPlannerName->text() );
      pathPlannerModel->zOffset = ui->dsbPathlPlannerZOffset->value();
      pathPlannerModel->viewBox = ui->dsbPathlPlannerViewbox->value();

      pathPlannerModel->individualRayColor = ui->cbPathPlannerRayColor->isChecked();
      pathPlannerModel->individualSegmentColor = ui->cbPathPlannerSegmentColor->isChecked();
      pathPlannerModel->bisectorsVisible = ui->cbPathPlannerBisectors->isChecked();

      pathPlannerModel->refreshColors();
    }
  }
}

void SettingsDialog::emitAllConfigSignals() {
  emit setGrid( ui->gbGrid->isChecked() );
  emitGridSettings();

  emit plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );

  emit cameraSmoothingChanged( ui->slCameraSmoothingOrientation->value(), ui->slCameraSmoothingPosition->value() );
}

QTreeWidget* SettingsDialog::getBlockTreeWidget() {
  return ui->twBlocks;
}

void SettingsDialog::on_cbSaveConfigOnExit_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "SaveConfigOnExit" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbLoadConfigOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "LoadConfigOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "OpenSettingsDialogOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbRunSimulatorOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "RunSimulatorOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_pbDeleteSettings_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.clear();
  settings.sync();
}

void SettingsDialog::on_pbSaveAsDefault_clicked() {
  saveDefaultConfig();
}

void SettingsDialog::on_pbLoadSavedConfig_clicked() {
  loadDefaultConfig();
}

void SettingsDialog::on_pbSetStringToFilename_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    QString dir = ui->twValues->model()->data( index, Qt::DisplayRole ).toString();
    QString fileName = QFileDialog::getOpenFileName( this,
                       tr( "Set Filename to String" ),
                       dir );

    if( !fileName.isEmpty() ) {
      ui->twValues->model()->setData( index, fileName );
    }
  }

  // as the size of the cell is most likely not enough for the filename, resize it
  ui->twValues->resizeColumnsToContents();
}

void SettingsDialog::on_pbComPortRefresh_clicked() {
  ui->cbComPorts->clear();

#ifdef SERIALPORT_ENABLED
  const auto infos = QSerialPortInfo::availablePorts();

  for( const QSerialPortInfo& info : infos ) {
    ui->cbComPorts->addItem( info.portName() );
  }

#endif
}

void SettingsDialog::on_pbBaudrateRefresh_clicked() {
  ui->cbBaudrate->clear();

#ifdef SERIALPORT_ENABLED
  const auto baudrates = QSerialPortInfo::standardBaudRates();

  for( const qint32& baudrate : baudrates ) {
    ui->cbBaudrate->addItem( QString::number( baudrate ) );
  }

#endif
}

void SettingsDialog::on_pbComPortSet_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    ui->twValues->model()->setData( index, ui->cbComPorts->currentText() );
  }
}

void SettingsDialog::on_pbBaudrateSet_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    ui->twValues->model()->setData( index, ui->cbBaudrate->currentText() );
  }
}

void SettingsDialog::on_pbClear_clicked() {
  // select all items, so everything gets deleted with a call to on_pbDeleteSelected_clicked()
  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    item->setSelected( true );
  }

  on_pbDeleteSelected_clicked();
}

void SettingsDialog::on_cbImplements_currentIndexChanged( int index ) {
  QModelIndex idx = ui->cbImplements->model()->index( index, 1 );
  QVariant data = ui->cbImplements->model()->data( idx );
  auto* block = qvariant_cast<QNEBlock*>( data );

  implementSectionModel->setDatasource( block );
  ui->twSections->resizeColumnsToContents();
}

void SettingsDialog::implementModelReset() {
  if( ui->cbImplements->currentIndex() == -1 || ui->cbImplements->currentIndex() >= ui->cbImplements->model()->rowCount() ) {
    ui->cbImplements->setCurrentIndex( 0 );
  }

  on_cbImplements_currentIndexChanged( ui->cbImplements->currentIndex() );
}

void SettingsDialog::pathPlannerModelReset() {
  if( ui->cbPathPlanner->currentIndex() == -1 || ui->cbPathPlanner->currentIndex() >= ui->cbPathPlanner->model()->rowCount() ) {
    ui->cbPathPlanner->setCurrentIndex( 0 );
  }

  on_cbPathPlanner_currentIndexChanged( ui->cbPathPlanner->currentIndex() );
}

void SettingsDialog::allModelsReset() {
  vectorBlockModel->resetModel();
  numberBlockModel->resetModel();
  actionBlockModel->resetModel();
  sliderBlockModel->resetModel();
  stringBlockModel->resetModel();
  implementBlockModel->resetModel();
  pathPlannerModelBlockModel->resetModel();
  meterModel->resetModel();
  plotBlockModel->resetModel();
  transmissionBlockModel->resetModel();

  ui->twValues->resizeColumnsToContents();
  ui->tvMeter->resizeColumnsToContents();
  ui->tvPlots->resizeColumnsToContents();
  ui->tvTransmission->resizeColumnsToContents();
  ui->twSections->resizeColumnsToContents();
}

void SettingsDialog::on_btnSectionAdd_clicked() {
  if( !( ui->twSections->selectionModel()->selection().indexes().isEmpty() ) ) {
    ui->twSections->model()->insertRow( ui->twSections->selectionModel()->selection().indexes().constFirst().row() );
  } else {
    ui->twSections->model()->insertRow( ui->twSections->model()->rowCount() );
  }
}

void SettingsDialog::on_btnSectionRemove_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector<int> rows;

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    rows.push_back( index.row() );
  }

  // sort to reverse order
  std::sort( rows.begin(), rows.end(), std::greater<>() );

  for( const auto& i : rows ) {
    ui->twSections->model()->removeRows( i, 1 );
  }
}

void SettingsDialog::on_pbSetSelectedCellsToNumber_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    ui->twSections->model()->setData( index, ui->dsbSectionsNumber->value() );
  }
}

void SettingsDialog::on_btnSectionMoveUp_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector<int> rows;

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    rows.push_back( index.row() );
  }

  // forward sort
  std::sort( rows.begin(), rows.end(), std::less<>() );

  auto* implementModel = qobject_cast<ImplementSectionModel*>( ui->twSections->model() );

  if( implementModel != nullptr ) {
    if( rows[0] > 0 ) {
      for( const auto& i : rows ) {
        implementModel->swapElements( i, i - 1 );
      }
    }
  }
}

void SettingsDialog::on_btnSectionMoveDown_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector<int> rows;

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    rows.push_back( index.row() );
  }

  // reverse sort
  std::sort( rows.begin(), rows.end(), std::greater<>() );

  auto* implementModel = qobject_cast<ImplementSectionModel*>( ui->twSections->model() );

  if( implementModel != nullptr ) {
    if( ( rows[0] + 1 ) < implementModel->rowCount() ) {
      for( const auto& i : rows ) {
        implementModel->swapElements( i, i + 1 );
      }
    }
  }
}

void SettingsDialog::on_dsbGridXStepCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void SettingsDialog::on_dsbGridYStepCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void SettingsDialog::on_dsbGridCameraThreshold_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void SettingsDialog::on_dsbGridCameraThresholdCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void SettingsDialog::emitGridSettings() {
  emit setGridValues( double( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::emitNoiseStandartDeviations() {
  emit noiseStandartDeviationsChanged( ui->dsbSimNoisePositionXY->value(),
                                       ui->dsbSimNoisePositionZ->value(),
                                       ui->dsbSimNoiseOrientation->value(),
                                       ui->dsbSimNoiseAccelerometer->value(),
                                       ui->dsbSimNoiseGyro->value() );
}

void SettingsDialog::on_pbColorCoarse_clicked() {
  const QColor color = QColorDialog::getColor( gridColorCoarse, this, QStringLiteral( "Select Grid Color" ) );

  if( color.isValid() ) {
    gridColorCoarse = color;
    ui->lbColorCoarse->setText( gridColorCoarse.name() );
    ui->lbColorCoarse->setPalette( QPalette( gridColorCoarse ) );
    ui->lbColorCoarse->setAutoFillBackground( true );

    saveGridValuesInSettings();
    emitGridSettings();
  }
}

void SettingsDialog::on_sbPathsInReserve_valueChanged( int ) {
  savePathPlannerValuesInSettings();
  emit plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );
}

void SettingsDialog::on_cbRestoreDockPositions_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "RestoreDockPositionsOnStart" ), checked );
  settings.sync();
}

void SettingsDialog::on_cbSaveDockPositionsOnExit_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "SaveDockPositionsOnExit" ), checked );
  settings.sync();
}

void SettingsDialog::on_pbMeterDefaults_clicked() {
  QItemSelection selection( ui->tvMeter->selectionModel()->selection() );

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    ui->tvMeter->model()->setData( index.siblingAtColumn( 1 ), true );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 2 ), "" );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 3 ), 1 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 4 ), 1 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 5 ), 0 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 6 ), QFont( QStringLiteral( "Monospace" ) ) );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 7 ), 20 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 8 ), true );
  }
}

void SettingsDialog::on_rbCrsSimulatorTransverseMercator_toggled( bool checked ) {
  geographicConvertionWrapperSimulator->useTM = checked;
}

void SettingsDialog::on_rbCrsGuidanceTransverseMercator_toggled( bool checked ) {
  geographicConvertionWrapperGuidance->useTM = checked;
}

void SettingsDialog::on_pbSaveAll_clicked() {
  // select all items, so everything gets saved
  {
    const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

    for( const auto& item : constRefOfList ) {
      item->setSelected( true );
    }
  }

  on_pbSaveSelected_clicked();

  // deselect all items
  {
    const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

    for( const auto& item : constRefOfList ) {
      item->setSelected( false );
    }
  }
}

void SettingsDialog::on_pbSaveDockPositionsAsDefault_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );
  KDDockWidgets::LayoutSaver saver;

  settings.setValue( QStringLiteral( "SavedDocks" ), saver.serializeLayout() );
  settings.sync();
}

void SettingsDialog::on_pbSaveDockPositions_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json)" );
  QString dir;
  QString fileName = QFileDialog::getSaveFileName( this,
                     tr( "Open Saved Config" ),
                     dir,
                     tr( "All Files (*);;JSON Files (*.json)" ),
                     &selectedFilter );

  if( !fileName.isEmpty() ) {

    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning() << "Couldn't open save file.";
      return;
    }

    QJsonObject jsonObject;
    QJsonObject jsonObjectDocks;

    KDDockWidgets::LayoutSaver saver;
    jsonObjectDocks[QStringLiteral( "SavedDocks" )] = QJsonValue::fromVariant( saver.serializeLayout() );

    jsonObject[QStringLiteral( "docks" )] = jsonObjectDocks;

    QJsonDocument jsonDocument( jsonObject );
    saveFile.write( jsonDocument.toJson() );
  }
}

void SettingsDialog::on_pbLoadDockPositions_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json)" );
  QString dir;

  auto* fileDialog = new QFileDialog( this,
                                      tr( "Open Saved Config" ),
                                      dir,
                                      selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;JSON Files (*.json)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect to
  // the signals instead of using the static functions for the dialogs
  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, [fileDialog]( const QString & fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        QByteArray saveData = loadFile.readAll();

        QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
        QJsonObject json = loadDoc.object();

        if( json.contains( QStringLiteral( "docks" ) ) ) {
          QJsonObject docksObject = json[QStringLiteral( "docks" )].toObject();

          KDDockWidgets::LayoutSaver saver;
          saver.restoreLayout( docksObject[QStringLiteral( "SavedDocks" )].toVariant().toByteArray() );
        }
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );

  // connect finished to deleteLater, so the dialog gets deleted when Cancel is pressed
  QObject::connect( fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater );

  fileDialog->open();
}

void SettingsDialog::on_sbGlobalPlannerMaxDeviation_valueChanged( double ) {
  savePathPlannerValuesInSettings();
  emit plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );
}

void SettingsDialog::on_rbMaterialAuto_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), true );
  settings.setValue( QStringLiteral( "Material/usePhong" ), true );
  settings.sync();
}

void SettingsDialog::on_rbMaterialPRB_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), true );
  settings.setValue( QStringLiteral( "Material/usePhong" ), false );
  settings.sync();
}

void SettingsDialog::on_rbMaterialPhong_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), false );
  settings.setValue( QStringLiteral( "Material/usePhong" ), true );
  settings.sync();
}

void SettingsDialog::on_cbPathPlanner_currentIndexChanged( int index ) {
  QModelIndex idx = ui->cbPathPlanner->model()->index( index, 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {

    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {

      ui->gbPathPlanner->blockSignals( true );
      ui->lePathPlannerName->blockSignals( true );
      ui->dsbPathlPlannerZOffset->blockSignals( true );
      ui->dsbPathlPlannerViewbox->blockSignals( true );
      ui->cbPathPlannerRayColor->blockSignals( true );
      ui->cbPathPlannerSegmentColor->blockSignals( true );
      ui->cbPathPlannerBisectors->blockSignals( true );

      ui->gbPathPlanner->setChecked( pathPlannerModel->visible );
      ui->lePathPlannerName->setText( block->getName() );
      ui->dsbPathlPlannerZOffset->setValue( pathPlannerModel->zOffset );
      ui->dsbPathlPlannerViewbox->setValue( pathPlannerModel->viewBox );

      ui->lbPathPlannerLineColor->setText( pathPlannerModel->linesColor.name() );
      ui->lbPathPlannerLineColor->setPalette( pathPlannerModel->linesColor );
      ui->lbPathPlannerLineColor->setAutoFillBackground( true );

      ui->lbPathPlannerRayColor->setText( pathPlannerModel->raysColor.name() );
      ui->lbPathPlannerRayColor->setPalette( pathPlannerModel->raysColor );
      ui->lbPathPlannerRayColor->setAutoFillBackground( true );

      ui->lbPathPlannerSegmentColor->setText( pathPlannerModel->segmentsColor.name() );
      ui->lbPathPlannerSegmentColor->setPalette( pathPlannerModel->segmentsColor );
      ui->lbPathPlannerSegmentColor->setAutoFillBackground( true );

      ui->lbPathPlannerBisectorsColor->setText( pathPlannerModel->bisectorsColor.name() );
      ui->lbPathPlannerBisectorsColor->setPalette( pathPlannerModel->bisectorsColor );
      ui->lbPathPlannerBisectorsColor->setAutoFillBackground( true );

      ui->cbPathPlannerRayColor->setChecked( pathPlannerModel->individualRayColor );
      ui->cbPathPlannerSegmentColor->setChecked( pathPlannerModel->individualSegmentColor );
      ui->cbPathPlannerBisectors->setChecked( pathPlannerModel->bisectorsVisible );

      ui->gbPathPlanner->blockSignals( false );
      ui->lePathPlannerName->blockSignals( false );
      ui->dsbPathlPlannerZOffset->blockSignals( false );
      ui->dsbPathlPlannerViewbox->blockSignals( false );
      ui->cbPathPlannerRayColor->blockSignals( false );
      ui->cbPathPlannerSegmentColor->blockSignals( false );
      ui->cbPathPlannerBisectors->blockSignals( false );
    }
  }
};

void SettingsDialog::on_lePathPlannerName_textChanged( const QString& ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_dsbPathlPlannerZOffset_valueChanged( double ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_cbPathPlannerRayColor_stateChanged( int ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_cbPathPlannerSegmentColor_stateChanged( int ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_dsbPathlPlannerViewbox_valueChanged( double ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_gbPathPlanner_toggled( bool ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_cbPathPlannerBisectors_stateChanged( int ) {
  setPathPlannerSettings();
}

void SettingsDialog::on_pbPathPlannerLineColor_clicked() {
  QModelIndex idx = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {
    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {
      const QColor color = QColorDialog::getColor( pathPlannerModel->linesColor, this, QStringLiteral( "Select Line Color" ) );

      if( color.isValid() ) {
        pathPlannerModel->linesColor = color;

        ui->lbPathPlannerLineColor->setText( color.name() );
        ui->lbPathPlannerLineColor->setPalette( color );
        ui->lbPathPlannerLineColor->setAutoFillBackground( true );

        pathPlannerModel->refreshColors();
      }
    }
  }
}

void SettingsDialog::on_pbPathPlannerRayColor_clicked() {
  QModelIndex idx = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {
    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {
      const QColor color = QColorDialog::getColor( pathPlannerModel->raysColor, this, QStringLiteral( "Select Ray Color" ) );

      if( color.isValid() ) {
        pathPlannerModel->raysColor = color;

        ui->lbPathPlannerRayColor->setText( color.name() );
        ui->lbPathPlannerRayColor->setPalette( color );
        ui->lbPathPlannerRayColor->setAutoFillBackground( true );

        pathPlannerModel->refreshColors();
      }
    }
  }
}

void SettingsDialog::on_pbPathPlannerSegmentColor_clicked() {
  QModelIndex idx = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {
    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {
      const QColor color = QColorDialog::getColor( pathPlannerModel->segmentsColor, this, QStringLiteral( "Select Segment Color" ) );

      if( color.isValid() ) {
        pathPlannerModel->segmentsColor = color;

        ui->lbPathPlannerSegmentColor->setText( color.name() );
        ui->lbPathPlannerSegmentColor->setPalette( color );
        ui->lbPathPlannerSegmentColor->setAutoFillBackground( true );

        pathPlannerModel->refreshColors();
      }
    }
  }
}

void SettingsDialog::on_pbPathPlannerBisectorsColor_clicked() {
  QModelIndex idx = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  QVariant data = ui->cbPathPlanner->model()->data( idx );

  if( auto* block = qvariant_cast<QNEBlock*>( data ) ) {
    if( auto* pathPlannerModel = qobject_cast<PathPlannerModel*>( block->object ) ) {
      const QColor color = QColorDialog::getColor( pathPlannerModel->bisectorsColor, this, QStringLiteral( "Select Bisectors Color" ) );

      if( color.isValid() ) {
        pathPlannerModel->bisectorsColor = color;

        ui->lbPathPlannerBisectorsColor->setText( color.name() );
        ui->lbPathPlannerBisectorsColor->setPalette( color );
        ui->lbPathPlannerBisectorsColor->setAutoFillBackground( true );

        pathPlannerModel->refreshColors();
      }
    }
  }
}

void SettingsDialog::on_dsbGamma_valueChanged( double arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Qt3D/Gamma" ), arg1 );
  settings.sync();

  qt3dWindow->defaultFrameGraph()->setGamma( float( arg1 ) );
}

void SettingsDialog::on_cbShowDebugOverlay_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Qt3D/ShowDebugOverlay" ), checked );
  settings.sync();

  qt3dWindow->defaultFrameGraph()->setShowDebugOverlay( checked );
}

void SettingsDialog::on_pbGammaDefault_clicked() {
  ui->dsbGamma->setValue( 2.2 );
}

void SettingsDialog::on_pbPlotsDefaults_clicked() {
  QItemSelection selection( ui->tvPlots->selectionModel()->selection() );

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    ui->tvPlots->model()->setData( index.siblingAtColumn( 1 ), false );
    ui->tvPlots->model()->setData( index.siblingAtColumn( 2 ), true );
    ui->tvPlots->model()->setData( index.siblingAtColumn( 3 ), QString() );
    ui->tvPlots->model()->setData( index.siblingAtColumn( 4 ), true );
    ui->tvPlots->model()->setData( index.siblingAtColumn( 5 ), 20 );
  }
}

void SettingsDialog::on_dsbSimNoisePositionXY_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void SettingsDialog::on_dsbSimNoiseGyro_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void SettingsDialog::on_dsbSimNoisePositionZ_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void SettingsDialog::on_dsbSimNoiseOrientation_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void SettingsDialog::on_dsbSimNoiseAccelerometer_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void SettingsDialog::on_slCameraSmoothingOrientation_valueChanged( int value ) {
  emit cameraSmoothingChanged( value, ui->slCameraSmoothingPosition->value() );
}

void SettingsDialog::on_slCameraSmoothingPosition_valueChanged( int value ) {
  emit cameraSmoothingChanged( ui->slCameraSmoothingOrientation->value(), value );
}

void SettingsDialog::on_twBlocks_itemDoubleClicked( QTreeWidgetItem* item, int ) {
  auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<QObject*>( item->data( 0, Qt::UserRole ) ) );

  if( factory != nullptr ) {
    QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene() );
    block->setPos( ui->gvNodeEditor->mapToScene( ui->gvNodeEditor->viewport()->rect().center() ) );
  }

  allModelsReset();
}
