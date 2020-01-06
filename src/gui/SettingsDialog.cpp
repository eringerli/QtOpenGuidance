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

#include "../block/VectorObject.h"
#include "../block/NumberObject.h"
#include "../block/StringObject.h"

#include "../block/Implement.h"

#include "../block/CameraController.h"
#include "../block/TractorModel.h"
#include "../block/TrailerModel.h"
#include "../block/SprayerModel.h"
#include "../block/GridModel.h"

#include "../block/AckermannSteering.h"

#include "../block/PoseSimulation.h"

#ifdef SPNAV_ENABLED
#include "SpaceNavigatorPollingThread.h"
#endif

#include "../block/PoseSynchroniser.h"

#include "../block/NmeaParserGGA.h"
#include "../block/NmeaParserHDT.h"
#include "../block/NmeaParserRMC.h"
#include "../block/TransverseMercatorConverter.h"

#include "../block/GuidancePlannerGui.h"
#include "../block/GuidanceGlobalPlanner.h"
#include "../block/GuidanceLocalPlanner.h"
#include "../block/GuidanceStanley.h"
#include "../block/GuidanceXte.h"
#include "../block/GuidanceGlobalPlannerModel.h"

#include "../block/DebugSink.h"
#include "../block/PrintLatency.h"

#include "../block/UdpSocket.h"
#include "../block/FileStream.h"
#include "../block/CommunicationPgn7FFE.h"
#include "../block/CommunicationJrk.h"

#include "../kinematic/TransverseMercatorWrapper.h"
#include "../kinematic/FixedKinematic.h"
#include "../kinematic/TrailerKinematic.h"

#include "../cgalKernel.h"

SettingsDialog::SettingsDialog( Qt3DCore::QEntity* rootEntity, QMainWindow* mainWindow, QWidget* parent ) :
  QDialog( parent ),
  mainWindow( mainWindow ),
  ui( new Ui::SettingsDialog ) {
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

    // global/local planner
    {
      ui->gbGlobalPlanner->setChecked( settings.value( QStringLiteral( "GlobalPlannerGraphics/Enabled" ), true ).toBool() );
      ui->slGlobalPlannerArrowSize->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/ArrowHeight" ), 50 ).toInt() );
      ui->slGlobalPlannerArrowWidth->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/ArrowWidth" ), 90 ).toInt() );
      ui->dsbGlobalPlannerTextureSize->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/TextureSize" ), 3 ).toDouble() );
      ui->slGlobalPlannerCenterLine->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/CenterLineSize" ), 0 ).toInt() );
      ui->slGlobalPlannerBorderLine->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/BorderLineSize" ), 1 ).toInt() );

      ui->cbGlobalPlannerBackground->setChecked( settings.value( QStringLiteral( "GlobalPlannerGraphics/BackgroundEnabled" ), true ).toBool() );

      globalPlannerArrowColor = settings.value( QStringLiteral( "GlobalPlannerGraphics/ArrowColor" ), QColor( 0xae, 0xec, 0x7f ) ).value<QColor>();
      globalPlannerCenterLineColor = settings.value( QStringLiteral( "GlobalPlannerGraphics/CenterLineColor" ), QColor( 0x7b, 0x5e, 0x9f ) ).value<QColor>();
      globalPlannerBorderLineColor = settings.value( QStringLiteral( "GlobalPlannerGraphics/BorderLineColor" ), QColor( 0x99, 0x99, 0 ) ).value<QColor>();
      globalPlannerBackgroundColor = settings.value( QStringLiteral( "GlobalPlannerGraphics/BackgroundColor" ), QColor( 0xff, 0xff, 0x7f ) ).value<QColor>();
      ui->slGlobalPlannerTransparency->setValue( settings.value( QStringLiteral( "GlobalPlannerGraphics/Transparency" ), 100 ).toInt() );


      ui->gbLocalPlanner->setChecked( settings.value( QStringLiteral( "LocalPlannerGraphics/Enabled" ), true ).toBool() );
      ui->dsbLocalPlannerArrowSize->setValue( settings.value( QStringLiteral( "LocalPlannerGraphics/ArrowSize" ), 1 ).toDouble() );
      ui->dsbLocalPlannerArrowDistance->setValue( settings.value( QStringLiteral( "LocalPlannerGraphics/ArrowDistance" ), 3 ).toDouble() );
      ui->dsbLocalPlannerLineWidth->setValue( settings.value( QStringLiteral( "LocalPlannerGraphics/LineWidth" ), 0.1 ).toDouble() );
      localPlannerArrowColor = settings.value( QStringLiteral( "LocalPlannerGraphics/ArrowColor" ), QColor( 0xff, 0x80, 0 ) ).value<QColor>();
      localPlannerLineColor = settings.value( QStringLiteral( "LocalPlannerGraphics/LineColor" ), QColor( 0xff, 0, 0 ) ).value<QColor>();
      ui->slLocalPlannerTransparency->setValue( settings.value( QStringLiteral( "LocalPlannerGraphics/Transparency" ), 100 ).toInt() );
    }

    // path planner
    {
      ui->sbPathsToGenerate->setValue( settings.value( QStringLiteral( "PathPlanner/PathsToGenerate" ), 5 ).toInt() );
      ui->sbPathsInReserve->setValue( settings.value( QStringLiteral( "PathPlanner/PathsInReserve" ), 3 ).toInt() );
    }

    blockSettingsSaving = false;
  }

  ui->gvNodeEditor->setDragMode( QGraphicsView::RubberBandDrag );

  auto* scene = new QGraphicsScene();
  ui->gvNodeEditor->setScene( scene );

  ui->gvNodeEditor->setRenderHint( QPainter::Antialiasing, true );

  auto* nodesEditor = new QNodesEditor( this );
  nodesEditor->install( scene );

  // Models for the tableview
  filterModelValues = new QSortFilterProxyModel( scene );
  vectorBlockModel = new VectorBlockModel( scene );
  numberBlockModel = new NumberBlockModel( scene );
  stringBlockModel = new StringBlockModel( scene );

  vectorBlockModel->addToCombobox( ui->cbValues );
  numberBlockModel->addToCombobox( ui->cbValues );
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

  meterModel = new MeterModel( scene );
  filterModelMeter = new QSortFilterProxyModel( scene );
  filterModelMeter->setDynamicSortFilter( true );
  filterModelMeter->setSourceModel( meterModel );
  filterModelMeter->sort( 0, Qt::AscendingOrder );
  ui->tvMeter->setModel( filterModelMeter );
  meterModelFontDelegate = new FontComboboxDelegate( ui->tvMeter );
  ui->tvMeter->setItemDelegateForColumn( 5, meterModelFontDelegate );

  // initialise the wrapper for the Transverse Mercator conversion, so all offsets are the same application-wide
  auto* tmw = new TransverseMercatorWrapper();

  // simulator
  poseSimulationFactory = new PoseSimulationFactory();
  poseSimulation = poseSimulationFactory->createNewObject();
  poseSimulationFactory->createBlock( ui->gvNodeEditor->scene(), poseSimulation );

  // SPNAV
#ifdef SPNAV_ENABLED
  qDebug() << "SPNAV_ENABLED";
  spaceNavigatorPollingThread = new SpaceNavigatorPollingThread( this );
  spaceNavigatorPollingThread->start();

  connect( spaceNavigatorPollingThread, SIGNAL( steerAngleChanged( float ) ), poseSimulation, SLOT( setSteerAngle( float ) ) );
  connect( spaceNavigatorPollingThread, SIGNAL( velocityChanged( float ) ), poseSimulation, SLOT( setVelocity( float ) ) );
#endif

  // guidance
  plannerGuiFactory = new PlannerGuiFactory( rootEntity );
  plannerGui = plannerGuiFactory->createNewObject();
  plannerGuiFactory->createBlock( ui->gvNodeEditor->scene(), plannerGui );

  globalPlannerFactory = new GlobalPlannerFactory( rootEntity, tmw );
  globalPlanner = globalPlannerFactory->createNewObject();
  globalPlannerFactory->createBlock( ui->gvNodeEditor->scene(), globalPlanner );

  QObject::connect( this, SIGNAL( plannerSettingsChanged( int, int ) ),
                    globalPlanner, SLOT( setPlannerSettings( int, int ) ) );

  localPlannerFactory = new LocalPlannerFactory();
  stanleyGuidanceFactory = new StanleyGuidanceFactory();
  xteGuidanceFactory = new XteGuidanceFactory();

  globalPlannerModelFactory = new GlobalPlannerModelFactory( rootEntity );
  globalPlannerModel = globalPlannerModelFactory->createNewObject();
  globalPlannerModelFactory->createBlock( ui->gvNodeEditor->scene(), globalPlannerModel );

  QObject::connect( this, SIGNAL( globalPlannerModelSettingsChanged( int, int, float, int, int, QColor, QColor, QColor, QColor ) ),
                    globalPlannerModel, SLOT( setPlannerModelSettings( int, int, float, int, int, QColor, QColor, QColor, QColor ) ) );

  // Factories for the blocks
  transverseMercatorConverterFactory = new TransverseMercatorConverterFactory( tmw );
  poseSynchroniserFactory = new PoseSynchroniserFactory();
  trailerModelFactory = new TrailerModelFactory( rootEntity );
  tractorModelFactory = new TractorModelFactory( rootEntity );
  sprayerModelFactory = new SprayerModelFactory( rootEntity );
  fixedKinematicFactory = new FixedKinematicFactory;
  trailerKinematicFactory = new TrailerKinematicFactory();
  vectorFactory = new VectorFactory( vectorBlockModel );
  numberFactory = new NumberFactory( numberBlockModel );
  stringFactory = new StringFactory( stringBlockModel );
  debugSinkFactory = new DebugSinkFactory();
  printLatencyFactory = new PrintLatencyFactory();
  udpSocketFactory = new UdpSocketFactory();

#ifdef SERIALPORT_ENABLED
  serialPortFactory = new SerialPortFactory();
#endif

  fileStreamFactory = new FileStreamFactory();
  communicationPgn7ffeFactory = new CommunicationPgn7ffeFactory();
  communicationJrkFactory = new CommunicationJrkFactory();
  nmeaParserGGAFactory = new NmeaParserGGAFactory();
  nmeaParserHDTFactory = new NmeaParserHDTFactory();
  nmeaParserRMCFactory = new NmeaParserRMCFactory();
  ackermannSteeringFactory = new AckermannSteeringFactory();

  vectorFactory->addToCombobox( ui->cbNodeType );
  numberFactory->addToCombobox( ui->cbNodeType );
  stringFactory->addToCombobox( ui->cbNodeType );
  fixedKinematicFactory->addToCombobox( ui->cbNodeType );
  tractorModelFactory->addToCombobox( ui->cbNodeType );
  trailerKinematicFactory->addToCombobox( ui->cbNodeType );
  trailerModelFactory->addToCombobox( ui->cbNodeType );
  sprayerModelFactory->addToCombobox( ui->cbNodeType );
  ackermannSteeringFactory->addToCombobox( ui->cbNodeType );
  poseSynchroniserFactory->addToCombobox( ui->cbNodeType );
  transverseMercatorConverterFactory->addToCombobox( ui->cbNodeType );
  xteGuidanceFactory->addToCombobox( ui->cbNodeType );
  stanleyGuidanceFactory->addToCombobox( ui->cbNodeType );
  localPlannerFactory->addToCombobox( ui->cbNodeType );
  nmeaParserGGAFactory->addToCombobox( ui->cbNodeType );
  nmeaParserHDTFactory->addToCombobox( ui->cbNodeType );
  nmeaParserRMCFactory->addToCombobox( ui->cbNodeType );
  debugSinkFactory->addToCombobox( ui->cbNodeType );
  udpSocketFactory->addToCombobox( ui->cbNodeType );

#ifdef SERIALPORT_ENABLED
  serialPortFactory->addToCombobox( ui->cbNodeType );
#endif

  fileStreamFactory->addToCombobox( ui->cbNodeType );
  communicationPgn7ffeFactory->addToCombobox( ui->cbNodeType );
  communicationJrkFactory->addToCombobox( ui->cbNodeType );
  printLatencyFactory->addToCombobox( ui->cbNodeType );

  // grid color picker
  ui->lbColor->setText( gridColor.name() );
  ui->lbColor->setPalette( QPalette( gridColor ) );
  ui->lbColor->setAutoFillBackground( true );
  ui->lbColorCoarse->setText( gridColorCoarse.name() );
  ui->lbColorCoarse->setPalette( QPalette( gridColorCoarse ) );
  ui->lbColorCoarse->setAutoFillBackground( true );

  setPlannerColorLabels();

  this->on_pbBaudrateRefresh_clicked();
  this->on_pbComPortRefresh_clicked();
}

SettingsDialog::~SettingsDialog() {
  delete ui;

  transverseMercatorConverterFactory->deleteLater();
  poseSynchroniserFactory->deleteLater();
  tractorModelFactory->deleteLater();
  trailerModelFactory->deleteLater();
  sprayerModelFactory->deleteLater();
  fixedKinematicFactory->deleteLater();
  trailerKinematicFactory->deleteLater();
  vectorFactory->deleteLater();
  numberFactory->deleteLater();
  stringFactory->deleteLater();
  debugSinkFactory->deleteLater();
  printLatencyFactory->deleteLater();
  udpSocketFactory->deleteLater();

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
  globalPlannerModelFactory->deleteLater();
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
    mainWindow->restoreState( settings.value( QStringLiteral( "SavedDockPositions" ) ).toByteArray() );
    mainWindow->restoreGeometry( settings.value( QStringLiteral( "SavedDockGeometry" ) ).toByteArray() );
  }
}

void SettingsDialog::onExit() {
  // save the current config if enabled
  if( ui->cbSaveConfigOnExit->isChecked() ) {
    saveDefaultConfig();
  }

  if( ui->cbSaveDockPositionsOnExit->isChecked() ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( QStringLiteral( "SavedDockPositions" ), mainWindow->saveState() );
    settings.setValue( QStringLiteral( "SavedDockGeometry" ), mainWindow->saveGeometry() );
    settings.sync();
  }
}

void SettingsDialog::loadDefaultConfig() {
  QFile saveFile( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/default.json" );

  if( saveFile.open( QIODevice::ReadOnly ) ) {
    loadConfigFromFile( saveFile );
  }
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
          int index = ui->cbNodeType->findText( blockObject[QStringLiteral( "type" )].toString(), Qt::MatchExactly );
          auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<QObject*>( ui->cbNodeType->itemData( index ) ) );

          if( factory != nullptr ) {
            BlockBase* obj = factory->createNewObject();
            QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene(), obj );

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

  // rescale the tableview
  ui->twValues->resizeColumnsToContents();
}

void SettingsDialog::on_pbAddBlock_clicked() {
  auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<BlockFactory*>( ui->cbNodeType->currentData() ) );

  if( factory != nullptr ) {
    BlockBase* obj = factory->createNewObject();
    QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene(), obj );
    block->setPos( ui->gvNodeEditor->mapToScene( ui->gvNodeEditor->viewport()->rect().center() ) );
  }

  allModelsReset();
}

void SettingsDialog::on_pbZoomOut_clicked() {
  ui->gvNodeEditor->zoomOut();
}

void SettingsDialog::on_pbZoomIn_clicked() {
  ui->gvNodeEditor->zoomIn();
}

void SettingsDialog::on_pbDeleteSelected_clicked() {
  const auto& constRefOfList = ui->gvNodeEditor->scene()->selectedItems();

  for( const auto& item : constRefOfList ) {
    auto* connection = qgraphicsitem_cast<QNEConnection*>( item );

    delete connection;

    {
      const auto& constRefOfList = ui->gvNodeEditor->scene()->selectedItems();

      for( const auto& item : constRefOfList ) {
        auto* block = qgraphicsitem_cast<QNEBlock*>( item );

        if( block != nullptr ) {
          if( !block->systemBlock ) {
            delete block;
            allModelsReset();
          }
        }
      }
    }
  }
}

void SettingsDialog:: on_gbGrid_toggled( bool arg1 ) {
  saveGridValuesInSettings();
  emit setGrid( bool( arg1 ) );
}

void SettingsDialog::on_dsbGridXStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( float( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::on_dsbGridYStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( float( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::on_dsbGridSize_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  emit setGridValues( float( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
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
    emit setGridValues( float( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
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

    settings.setValue( QStringLiteral( "PathPlanner/PathsToGenerate" ), ui->sbPathsToGenerate->value() );
    settings.setValue( QStringLiteral( "PathPlanner/PathsInReserve" ), ui->sbPathsInReserve->value() );
    settings.sync();
  }
}

void SettingsDialog::savePlannerValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/Enabled" ), ui->gbGlobalPlanner->isChecked() );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/ArrowHeight" ), ui->slGlobalPlannerArrowSize->value() );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/ArrowWidth" ), ui->slGlobalPlannerArrowWidth->value() );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/TextureSize" ), ui->dsbGlobalPlannerTextureSize->value() );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/CenterLineSize" ), ui->slGlobalPlannerCenterLine->value() );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/BorderLineSize" ), ui->slGlobalPlannerBorderLine->value() );

    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/BackgroundEnabled" ), ui->cbGlobalPlannerBackground->isChecked() );

    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/ArrowColor" ), globalPlannerArrowColor );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/CenterLineColor" ), globalPlannerCenterLineColor );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/BorderLineColor" ), globalPlannerBorderLineColor );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/BackgroundColor" ), globalPlannerBackgroundColor );
    settings.setValue( QStringLiteral( "GlobalPlannerGraphics/Transparency" ), ui->slGlobalPlannerTransparency->value() );

    settings.setValue( QStringLiteral( "LocalPlannerGraphics/Enabled" ), ui->gbLocalPlanner->isChecked() );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/ArrowSize" ), ui->dsbLocalPlannerArrowSize->value() );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/ArrowDistance" ), ui->dsbLocalPlannerArrowDistance->value() );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/LineWidth" ), ui->dsbLocalPlannerLineWidth->value() );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/ArrowColor" ), localPlannerArrowColor );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/LineColor" ), localPlannerLineColor );
    settings.setValue( QStringLiteral( "LocalPlannerGraphics/Transparency" ), ui->slLocalPlannerTransparency->value() );

    settings.sync();
  }
}

void SettingsDialog::setPlannerColorLabels() {
  QColor buffer = globalPlannerArrowColor;
  buffer.setAlphaF( 1 );
  ui->lbGlobalPlannerArrowColor->setText( buffer.name() );
  ui->lbGlobalPlannerArrowColor->setPalette( QPalette( buffer ) );
  ui->lbGlobalPlannerArrowColor->setAutoFillBackground( true );

  buffer = globalPlannerCenterLineColor;
  buffer.setAlphaF( 1 );
  ui->lbGlobalPlannerCenterLineColor->setText( buffer.name() );
  ui->lbGlobalPlannerCenterLineColor->setPalette( QPalette( buffer ) );
  ui->lbGlobalPlannerCenterLineColor->setAutoFillBackground( true );

  buffer = globalPlannerBorderLineColor;
  buffer.setAlphaF( 1 );
  ui->lbGlobalPlannerBorderLineColor->setText( buffer.name() );
  ui->lbGlobalPlannerBorderLineColor->setPalette( QPalette( buffer ) );
  ui->lbGlobalPlannerBorderLineColor->setAutoFillBackground( true );

  buffer = globalPlannerBackgroundColor;
  buffer.setAlphaF( 1 );
  ui->lbGlobalPlannerBackgroundColor->setText( buffer.name() );
  ui->lbGlobalPlannerBackgroundColor->setPalette( QPalette( buffer ) );
  ui->lbGlobalPlannerBackgroundColor->setAutoFillBackground( true );

  buffer = localPlannerLineColor;
  buffer.setAlphaF( 1 );
  ui->lbLocalPlannerLineColor->setText( buffer.name() );
  ui->lbLocalPlannerLineColor->setPalette( QPalette( buffer ) );
  ui->lbLocalPlannerLineColor->setAutoFillBackground( true );

  buffer = localPlannerArrowColor;
  buffer.setAlphaF( 1 );
  ui->lbLocalPlannerArrowColor->setText( buffer.name() );
  ui->lbLocalPlannerArrowColor->setPalette( QPalette( buffer ) );
  ui->lbLocalPlannerArrowColor->setAutoFillBackground( true );
}

void SettingsDialog::emitAllConfigSignals() {
  emit setGrid( ui->gbGrid->isChecked() );
  emitGridSettings();

  emit plannerSettingsChanged( ui->sbPathsToGenerate->value(), ui->sbPathsInReserve->value() );

  emit globalPlannerModelSetVisible( ui->gbGlobalPlanner->isChecked() );
  emitGlobalPlannerModelSettings();

  emit localPlannerModelSetVisible( ui->gbLocalPlanner->isChecked() );
  emitLocalPlannerModelSettings();
}

QComboBox* SettingsDialog::getCbNodeType() {
  return ui->cbNodeType;
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

void SettingsDialog::allModelsReset() {
  vectorBlockModel->resetModel();
  numberBlockModel->resetModel();
  stringBlockModel->resetModel();
  implementBlockModel->resetModel();
  meterModel->resetModel();
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
  emit setGridValues( float( ui->dsbGridXStep->value() ), float( ui->dsbGridYStep->value() ),
                      float( ui->dsbGridXStepCoarse->value() ), float( ui->dsbGridYStepCoarse->value() ),
                      float( ui->dsbGridSize->value() ), float( ui->dsbGridCameraThreshold->value() ), float( ui->dsbGridCameraThresholdCoarse->value() ),
                      gridColor, gridColorCoarse );
}

void SettingsDialog::emitGlobalPlannerModelSettings() {
  qreal transparencyFactor =  ui->slGlobalPlannerTransparency->value() / 100.0;
  globalPlannerArrowColor.setAlphaF( transparencyFactor );
  globalPlannerCenterLineColor.setAlphaF( transparencyFactor );
  globalPlannerBorderLineColor.setAlphaF( transparencyFactor );
  globalPlannerBackgroundColor.setAlphaF( ui->cbGlobalPlannerBackground->isChecked() ? transparencyFactor : 0 );

  emit globalPlannerModelSettingsChanged( ui->slGlobalPlannerArrowSize->value(),
                                          ui->slGlobalPlannerArrowWidth->value(),
                                          float( ui->dsbGlobalPlannerTextureSize->value() ),
                                          ui->slGlobalPlannerCenterLine->value(),
                                          ui->slGlobalPlannerBorderLine->value(),
                                          globalPlannerArrowColor,
                                          globalPlannerCenterLineColor,
                                          globalPlannerBorderLineColor,
                                          globalPlannerBackgroundColor );
}

void SettingsDialog::emitLocalPlannerModelSettings() {
  emit localPlannerModelSettingsChanged( float( ui->dsbLocalPlannerArrowSize->value() ),
                                         float( ui->dsbLocalPlannerArrowDistance->value() ),
                                         float( ui->dsbLocalPlannerLineWidth->value() ),
                                         localPlannerLineColor, localPlannerArrowColor );
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

void SettingsDialog::on_gbGlobalPlanner_toggled( bool ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_pbGlobalPlannerArrowColor_clicked() {
  QColor tmp = globalPlannerArrowColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Arrow Color" ) );

  if( color.isValid() ) {
    globalPlannerArrowColor = color;
    ui->lbGlobalPlannerArrowColor->setText( globalPlannerArrowColor.name() );
    ui->lbGlobalPlannerArrowColor->setPalette( QPalette( globalPlannerArrowColor ) );
    ui->lbGlobalPlannerArrowColor->setAutoFillBackground( true );

    emitGlobalPlannerModelSettings();
    savePlannerValuesInSettings();
  }
}

void SettingsDialog::on_pbGlobalPlannerBackgroundColor_clicked() {
  QColor tmp = globalPlannerBackgroundColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Background Color" ) );

  if( color.isValid() ) {
    globalPlannerBackgroundColor = color;
    ui->lbGlobalPlannerBackgroundColor->setText( globalPlannerBackgroundColor.name() );
    ui->lbGlobalPlannerBackgroundColor->setPalette( QPalette( globalPlannerBackgroundColor ) );
    ui->lbGlobalPlannerBackgroundColor->setAutoFillBackground( true );

    emitGlobalPlannerModelSettings();
    savePlannerValuesInSettings();
  }
}

void SettingsDialog::on_slGlobalPlannerTransparency_valueChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_cbGlobalPlannerBackground_stateChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}
void SettingsDialog::on_gbLocalPlanner_toggled( bool ) {
  savePlannerValuesInSettings();
}

void SettingsDialog::on_dsbLocalPlannerArrowSize_valueChanged( double ) {
  savePlannerValuesInSettings();
}

void SettingsDialog::on_dsbLocalPlannerLineWidth_valueChanged( double ) {
  savePlannerValuesInSettings();
}

void SettingsDialog::on_pbLocalPlannerArrowColor_clicked() {
  QColor tmp = localPlannerArrowColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Arrow Color" ) );

  if( color.isValid() ) {
    localPlannerArrowColor = color;
    ui->lbLocalPlannerArrowColor->setText( localPlannerArrowColor.name() );
    ui->lbLocalPlannerArrowColor->setPalette( QPalette( localPlannerArrowColor ) );
    ui->lbLocalPlannerArrowColor->setAutoFillBackground( true );

    savePlannerValuesInSettings();
  }

}

void SettingsDialog::on_pbLocalPlannerLineColor_clicked() {
  QColor tmp = localPlannerLineColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Line Color" ) );

  if( color.isValid() ) {
    localPlannerLineColor = color;
    ui->lbLocalPlannerLineColor->setText( localPlannerLineColor.name() );
    ui->lbLocalPlannerLineColor->setPalette( QPalette( localPlannerLineColor ) );
    ui->lbLocalPlannerLineColor->setAutoFillBackground( true );

    savePlannerValuesInSettings();
  }

}

void SettingsDialog::on_slLocalPlannerTransparency_valueChanged( int ) {
  savePlannerValuesInSettings();
}

void SettingsDialog::on_sbPathsToGenerate_valueChanged( int ) {
  savePathPlannerValuesInSettings();
  emit plannerSettingsChanged( ui->sbPathsToGenerate->value(), ui->sbPathsInReserve->value() );
}

void SettingsDialog::on_sbPathsInReserve_valueChanged( int ) {
  savePathPlannerValuesInSettings();
  emit plannerSettingsChanged( ui->sbPathsToGenerate->value(), ui->sbPathsInReserve->value() );
}

void SettingsDialog::on_pbGlobalPlannerCenterLineColor_clicked() {
  QColor tmp = globalPlannerCenterLineColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Background Color" ) );

  if( color.isValid() ) {
    globalPlannerCenterLineColor = color;
    ui->lbGlobalPlannerCenterLineColor->setText( globalPlannerCenterLineColor.name() );
    ui->lbGlobalPlannerCenterLineColor->setPalette( QPalette( globalPlannerCenterLineColor ) );
    ui->lbGlobalPlannerCenterLineColor->setAutoFillBackground( true );

    emitGlobalPlannerModelSettings();
    savePlannerValuesInSettings();
  }
}

void SettingsDialog::on_pbGlobalPlannerBorderLineColor_clicked() {
  QColor tmp = globalPlannerBorderLineColor;
  tmp.setAlphaF( 1 );
  const QColor color = QColorDialog::getColor( tmp, this, QStringLiteral( "Select Background Color" ) );

  if( color.isValid() ) {
    globalPlannerBorderLineColor = color;
    ui->lbGlobalPlannerBorderLineColor->setText( globalPlannerBorderLineColor.name() );
    ui->lbGlobalPlannerBorderLineColor->setPalette( QPalette( globalPlannerBorderLineColor ) );
    ui->lbGlobalPlannerBorderLineColor->setAutoFillBackground( true );

    emitGlobalPlannerModelSettings();
    savePlannerValuesInSettings();
  }
}

void SettingsDialog::on_slGlobalPlannerArrowSize_valueChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_slGlobalPlannerCenterLine_valueChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_slGlobalPlannerBorderLine_valueChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_dsbGlobalPlannerTextureSize_valueChanged( double ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
}

void SettingsDialog::on_slGlobalPlannerArrowWidth_valueChanged( int ) {
  emitGlobalPlannerModelSettings();
  savePlannerValuesInSettings();
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

void SettingsDialog::on_pbSaveDockPositions_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "SavedDockPositions" ), mainWindow->saveState() );
  settings.setValue( QStringLiteral( "SavedDockGeometry" ), mainWindow->saveGeometry() );
  settings.sync();
}

void SettingsDialog::on_pbMeterDefaults_clicked() {
  QItemSelection selection( ui->tvMeter->selectionModel()->selection() );

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    ui->tvMeter->model()->setData( index.siblingAtColumn( 1 ), true );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 2 ), 0 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 3 ), 1 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 4 ), 0 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 5 ), QFont( QStringLiteral( "Monospace" ) ) );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 6 ), 30 );
    ui->tvMeter->model()->setData( index.siblingAtColumn( 7 ), true );
  }
}
