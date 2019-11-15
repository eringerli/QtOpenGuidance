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
#include "../block/GridModel.h"

#include "../block/AckermannSteering.h"

#include "../block/PoseSimulation.h"

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

#include "../kinematic/FixedKinematic.h"
#include "../kinematic/TrailerKinematic.h"

#include "../kinematic/Tile.h"

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
      ui->cbSaveConfigOnExit->setCheckState( settings.value( "SaveConfigOnExit", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbLoadConfigOnStart->setCheckState( settings.value( "LoadConfigOnStart", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbOpenSettingsDialogOnStart->setCheckState( settings.value( "OpenSettingsDialogOnStart", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbRunSimulatorOnStart->setCheckState( settings.value( "RunSimulatorOnStart", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbRestoreDockPositions->setCheckState( settings.value( "RestoreDockPositionsOnStart", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbSaveDockPositionsOnExit->setCheckState( settings.value( "SaveDockPositionsOnExit", false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
    }

    // grid
    {
      ui->gbGrid->setChecked( settings.value( "Grid/Enabled", true ).toBool() );
      ui->dsbGridXStep->setValue( settings.value( "Grid/XStep", 1 ).toDouble() );
      ui->dsbGridYStep->setValue( settings.value( "Grid/YStep", 1 ).toDouble() );
      ui->dsbGridXStepCoarse->setValue( settings.value( "Grid/XStepCoarse", 10 ).toDouble() );
      ui->dsbGridYStepCoarse->setValue( settings.value( "Grid/YStepCoarse", 10 ).toDouble() );
      ui->dsbGridSize->setValue( settings.value( "Grid/Size", 10 ).toDouble() );
      ui->dsbGridCameraThreshold->setValue( settings.value( "Grid/CameraThreshold", 75 ).toDouble() );
      ui->dsbGridCameraThresholdCoarse->setValue( settings.value( "Grid/CameraThresholdCoarse", 250 ).toDouble() );
      gridColor = settings.value( "Grid/Color", QColor( 0x6b, 0x96, 0xa8 ) ).value<QColor>();
      gridColorCoarse = settings.value( "Grid/ColorCoarse", QColor( 0xa2, 0xe3, 0xff ) ).value<QColor>();
    }

    // tiles
    {
      ui->gbShowTiles->setChecked( settings.value( "Tile/Enabled", true ).toBool() );
      tileColor = settings.value( "Tile/Color", QColor( 0xff, 0xda, 0x21 ) ).value<QColor>();
    }

    // global/local planner
    {
      ui->gbGlobalPlanner->setChecked( settings.value( "GlobalPlannerGraphics/Enabled", true ).toBool() );
      ui->slGlobalPlannerArrowSize->setValue( settings.value( "GlobalPlannerGraphics/ArrowHeight", 50 ).toInt() );
      ui->slGlobalPlannerArrowWidth->setValue( settings.value( "GlobalPlannerGraphics/ArrowWidth", 90 ).toInt() );
      ui->dsbGlobalPlannerTextureSize->setValue( settings.value( "GlobalPlannerGraphics/TextureSize", 3 ).toDouble() );
      ui->slGlobalPlannerCenterLine->setValue( settings.value( "GlobalPlannerGraphics/CenterLineSize", 0 ).toInt() );
      ui->slGlobalPlannerBorderLine->setValue( settings.value( "GlobalPlannerGraphics/BorderLineSize", 1 ).toInt() );

      ui->cbGlobalPlannerBackground->setChecked( settings.value( "GlobalPlannerGraphics/BackgroundEnabled", true ).toBool() );

      globalPlannerArrowColor = settings.value( "GlobalPlannerGraphics/ArrowColor", QColor( 0xae, 0xec, 0x7f ) ).value<QColor>();
      globalPlannerCenterLineColor = settings.value( "GlobalPlannerGraphics/CenterLineColor", QColor( 0x7b, 0x5e, 0x9f ) ).value<QColor>();
      globalPlannerBorderLineColor = settings.value( "GlobalPlannerGraphics/BorderLineColor", QColor( 0x99, 0x99, 0 ) ).value<QColor>();
      globalPlannerBackgroundColor = settings.value( "GlobalPlannerGraphics/BackgroundColor", QColor( 0xff, 0xff, 0x7f ) ).value<QColor>();
      ui->slGlobalPlannerTransparency->setValue( settings.value( "GlobalPlannerGraphics/Transparency", 100 ).toInt() );


      ui->gbLocalPlanner->setChecked( settings.value( "LocalPlannerGraphics/Enabled", true ).toBool() );
      ui->dsbLocalPlannerArrowSize->setValue( settings.value( "LocalPlannerGraphics/ArrowSize", 1 ).toDouble() );
      ui->dsbLocalPlannerArrowDistance->setValue( settings.value( "LocalPlannerGraphics/ArrowDistance", 3 ).toDouble() );
      ui->dsbLocalPlannerLineWidth->setValue( settings.value( "LocalPlannerGraphics/LineWidth", 0.1 ).toDouble() );
      localPlannerArrowColor = settings.value( "LocalPlannerGraphics/ArrowColor", QColor( 0xff, 0x80, 0 ) ).value<QColor>();
      localPlannerLineColor = settings.value( "LocalPlannerGraphics/LineColor", QColor( 0xff, 0, 0 ) ).value<QColor>();
      ui->slLocalPlannerTransparency->setValue( settings.value( "LocalPlannerGraphics/Transparency", 100 ).toInt() );
    }

    // path planner
    {
      ui->sbPathsToGenerate->setValue( settings.value( "PathPlanner/PathsToGenerate", 5 ).toInt() );
      ui->sbPathsInReserve->setValue( settings.value( "PathPlanner/PathsInReserve", 3 ).toInt() );
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

  // initialise tiling
  Tile* tile = new Tile( &tileRoot, 0, 0, rootEntity, ui->gbShowTiles->isChecked(), tileColor );

  // initialise the wrapper for the Transverse Mercator conversion, so all offsets are the same application-wide
  auto* tmw = new TransverseMercatorWrapper();

  // simulator
  poseSimulationFactory = new PoseSimulationFactory( tile );
  poseSimulation = poseSimulationFactory->createNewObject();
  poseSimulationFactory->createBlock( ui->gvNodeEditor->scene(), poseSimulation );

  // guidance
  plannerGuiFactory = new PlannerGuiFactory( tile, rootEntity );
  plannerGui = plannerGuiFactory->createNewObject();
  plannerGuiFactory->createBlock( ui->gvNodeEditor->scene(), plannerGui );

  globalPlannerFactory = new GlobalPlannerFactory( tile, rootEntity );
  globalPlanner = globalPlannerFactory->createNewObject();
  globalPlannerFactory->createBlock( ui->gvNodeEditor->scene(), globalPlanner );

  QObject::connect( this, SIGNAL( plannerSettingsChanged( int, int ) ),
                    globalPlanner, SLOT( setPlannerSettings( int, int ) ) );

  localPlannerFactory = new LocalPlannerFactory( tile );
  stanleyGuidanceFactory = new StanleyGuidanceFactory( tile );
  xteGuidanceFactory = new XteGuidanceFactory( tile );

  globalPlannerModelFactory = new GlobalPlannerModelFactory( tile, rootEntity );
  globalPlannerModel = globalPlannerModelFactory->createNewObject();
  globalPlannerModelFactory->createBlock( ui->gvNodeEditor->scene(), globalPlannerModel );

  QObject::connect( this, SIGNAL( globalPlannerModelSettingsChanged( int, int, float, int, int, QColor, QColor, QColor, QColor ) ),
                    globalPlannerModel, SLOT( setPlannerModelSettings( int, int, float, int, int, QColor, QColor, QColor, QColor ) ) );

  // Factories for the blocks
  transverseMercatorConverterFactory = new TransverseMercatorConverterFactory( tile, tmw );
  poseSynchroniserFactory = new PoseSynchroniserFactory( tile );
  tractorModelFactory = new TractorModelFactory( rootEntity );
  trailerModelFactory = new TrailerModelFactory( rootEntity );
  fixedKinematicFactory = new FixedKinematicFactory;
  trailerKinematicFactory = new TrailerKinematicFactory( tile );
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
  implementFactory = new ImplementFactory( tile, implementBlockModel );

  vectorFactory->addToCombobox( ui->cbNodeType );
  numberFactory->addToCombobox( ui->cbNodeType );
  stringFactory->addToCombobox( ui->cbNodeType );
  fixedKinematicFactory->addToCombobox( ui->cbNodeType );
  tractorModelFactory->addToCombobox( ui->cbNodeType );
  trailerKinematicFactory->addToCombobox( ui->cbNodeType );
  trailerModelFactory->addToCombobox( ui->cbNodeType );
  ackermannSteeringFactory->addToCombobox( ui->cbNodeType );
  implementFactory->addToCombobox( ui->cbNodeType );
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

  // tile color picker
  ui->lbTileColor->setText( tileColor.name() );
  ui->lbTileColor->setPalette( QPalette( tileColor ) );
  ui->lbTileColor->setAutoFillBackground( true );
  tileRoot.setShowColor( tileColor );
  tileRoot.setShowEnable( ui->gbShowTiles->isChecked() );

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
  implementFactory->deleteLater();

  vectorBlockModel->deleteLater();
  numberBlockModel->deleteLater();
  stringBlockModel->deleteLater();

  implementBlockModel->deleteLater();
  implementSectionModel->deleteLater();

  poseSimulationFactory->deleteLater();

  poseSimulation->deleteLater();

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
    mainWindow->restoreState( settings.value( "SavedDockPositions" ).toByteArray() );
    mainWindow->restoreGeometry( settings.value( "SavedDockGeometry" ).toByteArray() );
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

    settings.setValue( "SavedDockPositions", mainWindow->saveState() );
    settings.setValue( "SavedDockGeometry", mainWindow->saveGeometry() );
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
    foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
      item->setSelected( true );
    }

    saveConfigToFile( saveFile );

    // deselect all items
    foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
      item->setSelected( false );
    }
  }
}

void SettingsDialog::on_cbValues_currentIndexChanged( int /*index*/ ) {
  auto* model = qobject_cast<QAbstractTableModel*>( qvariant_cast<QAbstractTableModel*>( ui->cbValues->currentData() ) );

  if( model ) {
    filterModelValues->setSourceModel( model );
    ui->twValues->resizeColumnsToContents();
  }

  ui->grpString->setHidden( !( ui->cbValues->currentText() == "String" ) );
  ui->grpNumber->setHidden( !( ui->cbValues->currentText() == "Number" ) );
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
  jsonObject["blocks"] = blocks;
  jsonObject["connections"] = connections;

  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    {
      auto* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block ) {
        block->toJSON( jsonObject );
      }
    }

    {
      auto* connection = qgraphicsitem_cast<QNEConnection*>( item );

      if( connection ) {
        connection->toJSON( jsonObject );
      }
    }
  }

  QJsonDocument jsonDocument( jsonObject );
  file.write( jsonDocument.toJson() );
}

QNEBlock* SettingsDialog::getBlockWithId( int id ) {
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( block->id == id ) {
        return block;
      }
    }
  }

  return nullptr;
}

QNEBlock* SettingsDialog::getBlockWithName( const QString& name ) {
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
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

  QFileDialog* fileDialog = new QFileDialog( this,
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
//    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#else
  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, [this, fileDialog]( QString fileName ) {
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
//    fileDialog->blockSignals( true );

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

  if( json.contains( "blocks" ) && json["blocks"].isArray() ) {
    QJsonArray blocksArray = json["blocks"].toArray();

    for( const auto& blockIndex : blocksArray ) {
      QJsonObject blockObject = blockIndex.toObject();
      int id = blockObject["id"].toInt( 0 );

      // if id is a system-id -> search the block and set the values
      if( id != 0 ) {

        // system id -> don't create new blocks
        if( id < int( QNEBlock::IdRange::UserIdStart ) ) {
          QNEBlock* block = getBlockWithName( blockObject["type"].toString() );

          if( block ) {
            idMap.insert( id, block->id );
            block->setX( blockObject["positionX"].toDouble( 0 ) );
            block->setY( blockObject["positionY"].toDouble( 0 ) );
          }

          // id is not a system-id -> create new blocks
        } else {
          int index = ui->cbNodeType->findText( blockObject["type"].toString(), Qt::MatchExactly );
          auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<QObject*>( ui->cbNodeType->itemData( index ) ) );

          if( factory ) {
            BlockBase* obj = factory->createNewObject();
            QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene(), obj );

            idMap.insert( id, block->id );

            block->setX( blockObject["positionX"].toDouble( 0 ) );
            block->setY( blockObject["positionY"].toDouble( 0 ) );
            block->setName( blockObject["name"].toString( factory->getNameOfFactory() ) );
            block->fromJSON( blockObject );
            block->setSelected( true );
          }
        }
      }
    }
  }


  if( json.contains( "connections" ) && json["connections"].isArray() ) {
    QJsonArray connectionsArray = json["connections"].toArray();

    for( auto&& connectionsIndex : connectionsArray ) {
      QJsonObject connectionsObject = connectionsIndex.toObject();

      if( !connectionsObject["idFrom"].isUndefined() &&
          !connectionsObject["idTo"].isUndefined() &&
          !connectionsObject["portFrom"].isUndefined() &&
          !connectionsObject["portTo"].isUndefined() ) {

        int idFrom = idMap[connectionsObject["idFrom"].toInt()];
        int idTo = idMap[connectionsObject["idTo"].toInt()];

        if( idFrom != 0 && idTo != 0 ) {
          QNEBlock* blockFrom = getBlockWithId( idFrom );
          QNEBlock* blockTo = getBlockWithId( idTo );

          if( blockFrom && blockTo ) {
            QString portFromName = connectionsObject["portFrom"].toString();
            QString portToName = connectionsObject["portTo"].toString();

            QNEPort* portFrom = blockFrom->getPortWithName( portFromName, true );
            QNEPort* portTo = blockTo->getPortWithName( portToName, false );

            if( portFrom && portTo ) {
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
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      block->emitConfigSignals();
    }
  }

  allModelsReset();

  // rescale the tableview
  ui->twValues->resizeColumnsToContents();
}

void SettingsDialog::on_pbAddBlock_clicked() {
  auto* factory = qobject_cast<BlockFactory*>( qvariant_cast<BlockFactory*>( ui->cbNodeType->currentData() ) );

  if( factory ) {
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
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    auto* connection = qgraphicsitem_cast<QNEConnection*>( item );

    delete connection;
  }

  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    auto* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( !block->systemBlock ) {
        delete block;
        allModelsReset();
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
  const QColor color = QColorDialog::getColor( gridColor, this, "Select Grid Color" );

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

    settings.setValue( "Grid/Enabled", bool( ui->gbGrid->isChecked() ) );
    settings.setValue( "Grid/XStep", ui->dsbGridXStep->value() );
    settings.setValue( "Grid/YStep", ui->dsbGridYStep->value() );
    settings.setValue( "Grid/XStepCoarse", ui->dsbGridXStepCoarse->value() );
    settings.setValue( "Grid/YStepCoarse", ui->dsbGridYStepCoarse->value() );
    settings.setValue( "Grid/Size", ui->dsbGridSize->value() );
    settings.setValue( "Grid/CameraThreshold", ui->dsbGridCameraThreshold->value() );
    settings.setValue( "Grid/CameraThresholdCoarse", ui->dsbGridCameraThresholdCoarse->value() );
    settings.setValue( "Grid/Color", gridColor );
    settings.sync();
  }
}

void SettingsDialog::savePathPlannerValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( "PathPlanner/PathsToGenerate", ui->sbPathsToGenerate->value() );
    settings.setValue( "PathPlanner/PathsInReserve", ui->sbPathsInReserve->value() );
    settings.sync();
  }
}

void SettingsDialog::saveTileValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( "Tile/Enabled", bool( ui->gbShowTiles->isChecked() ) );
    settings.setValue( "Tile/Color", tileColor );
    settings.sync();
  }
}

void SettingsDialog::savePlannerValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                        QSettings::IniFormat );

    settings.setValue( "GlobalPlannerGraphics/Enabled", ui->gbGlobalPlanner->isChecked() );
    settings.setValue( "GlobalPlannerGraphics/ArrowHeight", ui->slGlobalPlannerArrowSize->value() );
    settings.setValue( "GlobalPlannerGraphics/ArrowWidth", ui->slGlobalPlannerArrowWidth->value() );
    settings.setValue( "GlobalPlannerGraphics/TextureSize", ui->dsbGlobalPlannerTextureSize->value() );
    settings.setValue( "GlobalPlannerGraphics/CenterLineSize", ui->slGlobalPlannerCenterLine->value() );
    settings.setValue( "GlobalPlannerGraphics/BorderLineSize", ui->slGlobalPlannerBorderLine->value() );

    settings.setValue( "GlobalPlannerGraphics/BackgroundEnabled", ui->cbGlobalPlannerBackground->isChecked() );

    settings.setValue( "GlobalPlannerGraphics/ArrowColor", globalPlannerArrowColor );
    settings.setValue( "GlobalPlannerGraphics/CenterLineColor", globalPlannerCenterLineColor );
    settings.setValue( "GlobalPlannerGraphics/BorderLineColor", globalPlannerBorderLineColor );
    settings.setValue( "GlobalPlannerGraphics/BackgroundColor", globalPlannerBackgroundColor );
    settings.setValue( "GlobalPlannerGraphics/Transparency", ui->slGlobalPlannerTransparency->value() );

    settings.setValue( "LocalPlannerGraphics/Enabled", ui->gbLocalPlanner->isChecked() );
    settings.setValue( "LocalPlannerGraphics/ArrowSize", ui->dsbLocalPlannerArrowSize->value() );
    settings.setValue( "LocalPlannerGraphics/ArrowDistance", ui->dsbLocalPlannerArrowDistance->value() );
    settings.setValue( "LocalPlannerGraphics/LineWidth", ui->dsbLocalPlannerLineWidth->value() );
    settings.setValue( "LocalPlannerGraphics/ArrowColor", localPlannerArrowColor );
    settings.setValue( "LocalPlannerGraphics/LineColor", localPlannerLineColor );
    settings.setValue( "LocalPlannerGraphics/Transparency", ui->slLocalPlannerTransparency->value() );

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

  settings.setValue( "SaveConfigOnExit", bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbLoadConfigOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( "LoadConfigOnStart", bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( "OpenSettingsDialogOnStart", bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void SettingsDialog::on_cbRunSimulatorOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( "RunSimulatorOnStart", bool( arg1 == Qt::CheckState::Checked ) );
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
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
    item->setSelected( true );
  }

  on_pbDeleteSelected_clicked();
}

void SettingsDialog::on_pbTileColor_clicked() {
  const QColor color = QColorDialog::getColor( tileColor, this, "Select Grid Color" );

  if( color.isValid() ) {
    tileColor = color;
    ui->lbTileColor->setText( tileColor.name() );
    ui->lbTileColor->setPalette( QPalette( tileColor ) );
    ui->lbTileColor->setAutoFillBackground( true );

    tileRoot.setShowColor( tileColor );
    saveTileValuesInSettings();
  }
}

void SettingsDialog::on_gbShowTiles_toggled( bool enabled ) {
  tileRoot.setShowEnable( enabled );
  saveTileValuesInSettings();
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
}

void SettingsDialog::on_btnSectionAdd_clicked() {
  if( ui->twSections->selectionModel()->selection().indexes().count() ) {
    ui->twSections->model()->insertRow( ui->twSections->selectionModel()->selection().indexes().first().row() );
  } else {
    ui->twSections->model()->insertRow( ui->twSections->model()->rowCount() );
  }
}

void SettingsDialog::on_btnSectionRemove_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  QList<int> rows;

  foreach( const QModelIndex& index, selection.indexes() ) {
    rows.append( index.row() );
  }

  // sort to reverse order
  std::sort( rows.begin(), rows.end(), std::greater<int>() );

  foreach( const int& i, rows ) {
    ui->twSections->model()->removeRows( i, 1 );
  }
}

void SettingsDialog::on_pbSetSelectedCellsToNumber_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  foreach( const QModelIndex& index, selection.indexes() ) {
    ui->twSections->model()->setData( index, ui->dsbSectionsNumber->value() );
  }
}

void SettingsDialog::on_btnSectionMoveUp_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  QList<int> rows;

  foreach( const QModelIndex& index, selection.indexes() ) {
    rows.append( index.row() );
  }

  // forward sort
  std::sort( rows.begin(), rows.end(), std::less<int>() );

  auto* implementModel = qobject_cast<ImplementSectionModel*>( ui->twSections->model() );

  if( implementModel ) {
    if( rows[0] > 0 ) {
      foreach( const int& i, rows ) {
        implementModel->swapElements( i, i - 1 );
      }
    }
  }
}

void SettingsDialog::on_btnSectionMoveDown_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  QList<int> rows;

  foreach( const QModelIndex& index, selection.indexes() ) {
    rows.append( index.row() );
  }

  // reverse sort
  std::sort( rows.begin(), rows.end(), std::greater<int>() );

  auto* implementModel = qobject_cast<ImplementSectionModel*>( ui->twSections->model() );

  if( implementModel ) {
    if( ( rows[0] + 1 ) < implementModel->rowCount() ) {
      foreach( const int& i, rows ) {
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
  const QColor color = QColorDialog::getColor( gridColorCoarse, this, "Select Grid Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Arrow Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Background Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Arrow Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Line Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Background Color" );

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
  const QColor color = QColorDialog::getColor( tmp, this, "Select Background Color" );

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

  settings.setValue( "RestoreDockPositionsOnStart", checked );
  settings.sync();
}

void SettingsDialog::on_cbSaveDockPositionsOnExit_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( "SaveDockPositionsOnExit", checked );
  settings.sync();
}

void SettingsDialog::on_pbSaveDockPositions_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( "SavedDockPositions", mainWindow->saveState() );
  settings.setValue( "SavedDockGeometry", mainWindow->saveGeometry() );
  settings.sync();
}
