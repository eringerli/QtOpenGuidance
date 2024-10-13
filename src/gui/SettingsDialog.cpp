// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMap>

#include <QObject>
#include <QSignalBlocker>
#include <QtWidgets>

#include <QFileDialog>

#include <QDebug>

#include "block/BlockBase.h"
#include "gui/OpenSaveHelper.h"
#include "gui/model/FactoriesModel.h"
#include "qjsonobject.h"
#include "qnamespace.h"
#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "SettingsDialog.h"
#include "qsplitter.h"
#include "ui_SettingsDialog.h"

#include "MyMainWindow.h"
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/Qt3DWindow>
#include <memory>

#include "helpers/BlocksManager.h"
#include "helpers/FactoriesManager.h"

#include "block/sectionControl/Implement.h"

#include "block/kinematic/FixedKinematic.h"
#include "block/kinematic/FixedKinematicPrimitive.h"
#include "block/kinematic/TrailerKinematic.h"
#include "block/kinematic/TrailerKinematicPrimitive.h"
#include "helpers/GeographicConvertionWrapper.h"

#include "model/ActionDockBlockModel.h"
#include "model/ImplementBlockModel.h"
#include "model/ImplementSectionModel.h"
#include "model/NumberBlockModel.h"
#include "model/OrientationBlockModel.h"
#include "model/PathPlannerModelBlockModel.h"
#include "model/PlotBlockModel.h"
#include "model/SliderDockBlockModel.h"
#include "model/StringBlockModel.h"
#include "model/TransmissionBlockModel.h"
#include "model/ValueBlockModel.h"
#include "model/VectorBlockModel.h"

#include <QGraphicsScene>
#include <QSortFilterProxyModel>

#include "FontComboboxDelegate.h"
#include "toolbar/NewOpenSaveToolbar.h"

#include "helpers/RateLimiter.h"
#include "helpers/cgalHelper.h"

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>
#include <tuple>

#include <kddockwidgets/Config.h>
#include <kddockwidgets/LayoutSaver.h>

#include "qneblock.h"

#ifdef SERIALPORT_ENABLED
  #include <QSerialPortInfo>
#endif

SettingsDialog::SettingsDialog( MyMainWindow*           mainWindow,
                                Qt3DExtras::Qt3DWindow* qt3dWindow,
                                FactoriesManager*       factoriesManager,
                                BlocksManager*          blocksManager,
                                QWidget*                parent )
    : QDialog( parent )
    , mainWindow( mainWindow )
    , qt3dWindow( qt3dWindow )
    , factoriesManager( factoriesManager )
    , blocksManager( blocksManager )
    , ui( new Ui::SettingsDialog ) {
  ui->setupUi( this );

  // load states of checkboxes from global config
  {
    blockSettingsSaving = true;

    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

    // checkboxes
    {
      ui->cbSaveConfigOnExit->setCheckState(
        settings.value( QStringLiteral( "SaveConfigOnExit" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbLoadConfigOnStart->setCheckState(
        settings.value( QStringLiteral( "LoadConfigOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbOpenSettingsDialogOnStart->setCheckState( settings.value( QStringLiteral( "OpenSettingsDialogOnStart" ), false ).toBool() ?
                                                        Qt::CheckState::Checked :
                                                        Qt::CheckState::Unchecked );
      ui->cbRunSimulatorOnStart->setCheckState(
        settings.value( QStringLiteral( "RunSimulatorOnStart" ), false ).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
      ui->cbRestoreDockPositions->setCheckState( settings.value( QStringLiteral( "RestoreDockPositionsOnStart" ), false ).toBool() ?
                                                   Qt::CheckState::Checked :
                                                   Qt::CheckState::Unchecked );
      ui->cbSaveDockPositionsOnExit->setCheckState( settings.value( QStringLiteral( "SaveDockPositionsOnExit" ), false ).toBool() ?
                                                      Qt::CheckState::Checked :
                                                      Qt::CheckState::Unchecked );
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
      gridColor       = settings.value( QStringLiteral( "Grid/Color" ), QColor( 0x6b, 0x96, 0xa8 ) ).value< QColor >();
      gridColorCoarse = settings.value( QStringLiteral( "Grid/ColorCoarse" ), QColor( 0xa2, 0xe3, 0xff ) ).value< QColor >();
    }

    // path planner
    {
      ui->sbPathsInReserve->setValue( settings.value( QStringLiteral( "PathPlanner/PathsInReserve" ), 3 ).toInt() );
      ui->sbGlobalPlannerMaxDeviation->setValue( settings.value( QStringLiteral( "PathPlanner/MaxDeviation" ), 0.1 ).toDouble() );
    }

    // material
    {
      bool usePBR   = settings.value( QStringLiteral( "Material/usePBR" ), true ).toBool();
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
      double gamma            = settings.value( QStringLiteral( "Qt3D/Gamma" ), 2.2 ).toDouble();
      bool   showDebugOverlay = settings.value( QStringLiteral( "Qt3D/ShowDebugOverlay" ), false ).toBool();

      ui->dsbGamma->setValue( gamma );
      ui->cbShowDebugOverlay->setChecked( showDebugOverlay );
    }

    {
      ui->sbRatePlotDocks->setValue( settings.value( QStringLiteral( "RateLimit/Plot" ), 30 ).toInt() );
      ui->sbRateValueDocks->setValue( settings.value( QStringLiteral( "RateLimit/Value" ), 60 ).toInt() );
      ui->sbRate3dView->setValue( settings.value( QStringLiteral( "RateLimit/Graphics" ), 30 ).toInt() );

      RateLimiter::setRate( RateLimiter::Type::PlotDock, ui->sbRatePlotDocks->value() );
      RateLimiter::setRate( RateLimiter::Type::ValueDock, ui->sbRateValueDocks->value() );
      RateLimiter::setRate( RateLimiter::Type::Graphical, ui->sbRate3dView->value() );
    }

    // camera smoothing
    {
      ui->slCameraSmoothingOrientation->setValue( settings.value( QStringLiteral( "Camera/orientationSmoothing" ), 0.9 ).toFloat() * 100 );
      ui->slCameraSmoothingPosition->setValue( settings.value( QStringLiteral( "Camera/positionSmoothing" ), 0.5 ).toFloat() * 100 );
    }

    blockSettingsSaving = false;
  }

  ui->gvNodeEditor->setDragMode( QGraphicsView::RubberBandDrag );

  auto* scene = new QGraphicsScene();

  {
    auto font = scene->font();
    font.setPixelSize( 12 );
    scene->setFont( font );
  }

  ui->gvNodeEditor->setScene( scene );

  ui->gvNodeEditor->setRenderHint( QPainter::Antialiasing, true );

  auto* nodesEditor = new QNodesEditor( this );
  nodesEditor->install( scene );
  QObject::connect( nodesEditor, &QNodesEditor::resetModels, this, &SettingsDialog::resetAllModels );
  QObject::connect( nodesEditor, &QNodesEditor::deleteBlock, blocksManager, &BlocksManager::deleteBlock );
  QObject::connect( nodesEditor, &QNodesEditor::deleteConnection, blocksManager, &BlocksManager::deleteConnection );
  QObject::connect( nodesEditor, &QNodesEditor::createConnection, blocksManager, &BlocksManager::createConnection );

  // Models for the tableview
  filterModelValues     = new QSortFilterProxyModel( blocksManager );
  vectorBlockModel      = new VectorBlockModel( blocksManager );
  orientationBlockModel = new OrientationBlockModel( blocksManager );
  numberBlockModel      = new NumberBlockModel( blocksManager );
  actionBlockModel      = new ActionDockBlockModel( blocksManager );
  sliderBlockModel      = new SliderDockBlockModel( blocksManager );
  stringBlockModel      = new StringBlockModel( blocksManager );

  ui->cbValues->addItem( QStringLiteral( "Number Literals" ), QVariant::fromValue( numberBlockModel ) );
  ui->cbValues->addItem( QStringLiteral( "3D-Vector Literals" ), QVariant::fromValue( vectorBlockModel ) );
  ui->cbValues->addItem( QStringLiteral( "String Literals" ), QVariant::fromValue( stringBlockModel ) );
  ui->cbValues->addItem( QStringLiteral( "Action/State Dock" ), QVariant::fromValue( actionBlockModel ) );
  ui->cbValues->addItem( QStringLiteral( "Orientation Literals" ), QVariant::fromValue( orientationBlockModel ) );
  ui->cbValues->addItem( QStringLiteral( "Slider Dock" ), QVariant::fromValue( sliderBlockModel ) );

  filterModelValues->setSourceModel( numberBlockModel );
  filterModelValues->sort( 0, Qt::AscendingOrder );
  ui->twValues->setModel( filterModelValues );

  implementBlockModel   = new ImplementBlockModel( blocksManager );
  filterModelImplements = new QSortFilterProxyModel( blocksManager );
  filterModelImplements->setDynamicSortFilter( true );
  filterModelImplements->setSourceModel( implementBlockModel );
  filterModelImplements->sort( 0, Qt::AscendingOrder );
  ui->cbImplements->setModel( filterModelImplements );
  ui->cbImplements->setCurrentIndex( 0 );
  QObject::connect( implementBlockModel, &QAbstractItemModel::modelReset, this, &SettingsDialog::implementModelReset );

  implementSectionModel = new ImplementSectionModel();
  ui->twSections->setModel( implementSectionModel );

  meterModel       = new ValueBlockModel( blocksManager );
  filterModelMeter = new QSortFilterProxyModel( blocksManager );
  filterModelMeter->setDynamicSortFilter( true );
  filterModelMeter->setSourceModel( meterModel );
  filterModelMeter->sort( 0, Qt::AscendingOrder );
  ui->tvMeter->setModel( filterModelMeter );
  meterModelFontDelegate = new FontComboboxDelegate( ui->tvMeter );
  ui->tvMeter->setItemDelegateForColumn( 6, meterModelFontDelegate );

  plotBlockModel  = new PlotBlockModel( blocksManager );
  filterModelPlot = new QSortFilterProxyModel( blocksManager );
  filterModelPlot->setDynamicSortFilter( true );
  filterModelPlot->setSourceModel( plotBlockModel );
  filterModelPlot->sort( 0, Qt::AscendingOrder );
  ui->tvPlots->setModel( filterModelPlot );

  transmissionBlockModel        = new TransmissionBlockModel( blocksManager );
  filterTtransmissionBlockModel = new QSortFilterProxyModel( blocksManager );
  filterTtransmissionBlockModel->setDynamicSortFilter( true );
  filterTtransmissionBlockModel->setSourceModel( transmissionBlockModel );
  filterTtransmissionBlockModel->sort( 0, Qt::AscendingOrder );
  ui->tvTransmission->setModel( filterTtransmissionBlockModel );

  pathPlannerModelBlockModel  = new PathPlannerModelBlockModel( blocksManager );
  filterModelPathPlannerModel = new QSortFilterProxyModel( blocksManager );
  filterModelPathPlannerModel->setDynamicSortFilter( true );
  filterModelPathPlannerModel->setSourceModel( pathPlannerModelBlockModel );
  filterModelPathPlannerModel->sort( 0, Qt::AscendingOrder );
  ui->cbPathPlanner->setModel( filterModelPathPlannerModel );
  ui->cbPathPlanner->setCurrentIndex( 0 );
  QObject::connect( pathPlannerModelBlockModel, &QAbstractItemModel::modelReset, this, &SettingsDialog::pathPlannerModelReset );

  ui->twBlocks->setModel( new FactoriesModel( factoriesManager ) );

  QObject::connect( factoriesManager, &FactoriesManager::factoriesChanged, this, &SettingsDialog::resetAllModels );
  QObject::connect( blocksManager, &BlocksManager::objectsChanged, this, &SettingsDialog::resetBlockViewScene );
  QObject::connect( blocksManager, &BlocksManager::objectsChanged, this, &SettingsDialog::resetAllModels );

  // grid color picker
  ui->lbColor->setText( gridColor.name() );
  ui->lbColor->setPalette( QPalette( gridColor ) );
  ui->lbColor->setAutoFillBackground( true );
  ui->lbColorCoarse->setText( gridColorCoarse.name() );
  ui->lbColorCoarse->setPalette( QPalette( gridColorCoarse ) );
  ui->lbColorCoarse->setAutoFillBackground( true );

  this->on_pbBaudrateRefresh_clicked();
  this->on_pbComPortRefresh_clicked();

  mainWindow->readSettings();
}

SettingsDialog::~SettingsDialog() {
  delete ui;

  vectorBlockModel->deleteLater();
  numberBlockModel->deleteLater();
  stringBlockModel->deleteLater();
  sliderBlockModel->deleteLater();
  actionBlockModel->deleteLater();

  implementBlockModel->deleteLater();
  implementSectionModel->deleteLater();
}

QGraphicsScene*
SettingsDialog::getSceneOfConfigGraphicsView() {
  return ui->gvNodeEditor->scene();
}

void
SettingsDialog::toggleVisibility() {
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

void
SettingsDialog::onStart() {
  // save the current config if enabled
  if( ui->cbLoadConfigOnStart->isChecked() ) {
    loadDefaultConfig();
  }

  if( ui->cbRestoreDockPositions->isChecked() ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );
    KDDockWidgets::LayoutSaver saver;

    saver.restoreLayout( settings.value( QStringLiteral( "SavedDocks" ) ).toByteArray() );
  }

  ui->twBlocks->setSortingEnabled( true );
  ui->twBlocks->sortByColumn( 0, Qt::AscendingOrder );
}

void
SettingsDialog::onExit() {
  if( ui->cbSaveConfigOnExit->isChecked() ) {
    saveDefaultConfig();
  }

  if( ui->cbSaveDockPositionsOnExit->isChecked() ) {
    on_pbSaveDockPositionsAsDefault_clicked();
  }
}

void
SettingsDialog::loadDefaultConfig() {
  QFile saveFile( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/default.json" );

  if( saveFile.open( QIODevice::ReadOnly ) ) {
    loadConfigFromFile( saveFile );
  }
}

void
SettingsDialog::saveDefaultConfig() {
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

void
SettingsDialog::saveConfigAndDocks() {
  saveDefaultConfig();
  on_pbSaveDockPositionsAsDefault_clicked();
}

void
SettingsDialog::on_cbValues_currentIndexChanged( int /*index*/ ) {
  auto* model = qobject_cast< QAbstractTableModel* >( qvariant_cast< QAbstractTableModel* >( ui->cbValues->currentData() ) );

  if( model != nullptr ) {
    filterModelValues->setSourceModel( model );
    ui->twValues->resizeColumnsToContents();
  }

  ui->grpString->setHidden( !( ui->cbValues->currentText() == QLatin1StringView( "String" ) ) );
  ui->grpNumber->setHidden( !( ui->cbValues->currentText() == QLatin1StringView( "Number" ) ) );
}

void
SettingsDialog::on_pbSaveSelected_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json" );
  QString dir;
  QString fileName =
    QFileDialog::getSaveFileName( this, tr( "Open Saved Config" ), dir, tr( "All Files (*);;JSON Files (*.json)" ), &selectedFilter );

  if( !fileName.isEmpty() ) {
    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning() << "Couldn't open save file.";
      return;
    }

    saveConfigToFile( saveFile );
  }
}

// TODO
void
SettingsDialog::saveConfigToFile( QFile& file ) {
  //  QJsonObject jsonObject;

  //  auto jsonBlocks      = QJsonArray();
  //  auto jsonConnections = QJsonArray();

  std::vector< int >                       blocks;
  std::vector< BlockConnectionDefinition > connections;

  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();
  for( const auto& item : constRefOfList ) {
    {
      auto* block = qgraphicsitem_cast< QNEBlock* >( item );

      if( block != nullptr ) {
        blocks.push_back( block->block->id() );
      }
    }

    {
      auto* connection = qgraphicsitem_cast< QNEConnection* >( item );

      if( connection != nullptr ) {
        connections.emplace_back( connection->connection()->getDefinition() );
      }
    }
  }

  blocksManager->saveConfigToFile( blocks, connections, file );

  //  std::sort( blocks.begin(), blocks.end(), []( QNEBlock* first, QNEBlock* second ) { return first->block->id() < second->block->id(); }
  //  );

  //  std::sort( connections.begin(), connections.end(), []( QNEConnection* first, QNEConnection* second ) {
  //    if( first->port1()->block->block->id() == second->port1()->block->block->id() ) {
  //      if( first->port2()->block->block->id() == second->port2()->block->block->id() ) {
  //        return first->port2()->getName() < second->port2()->getName();
  //      }
  //      return first->port2()->block->block->id() < second->port2()->block->block->id();
  //    }
  //    return first->port1()->block->block->id() < second->port1()->block->block->id();
  //  } );

  //  //  for( const auto* block : blocks ) {
  //  //    QJsonObject json;
  //  //    block->toJSON( json );
  //  //    jsonBlocks.append( json );
  //  //  }

  //  //  for( const auto* connection : connections ) {
  //  //    QJsonObject json;
  //  //    connection->toJSON( json );
  //  //    jsonConnections.append( json );
  //  //  }

  //  jsonObject[QStringLiteral( "blocks" )]      = jsonBlocks;
  //  jsonObject[QStringLiteral( "connections" )] = jsonConnections;

  //  QJsonDocument jsonDocument( jsonObject );
  //  file.write( jsonDocument.toJson() );
}

// QNEBlock* SettingsDialog::getBlockWithId( int id ) {
//   const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

//  for( const auto& item : constRefOfList ) {
//    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

//    if( block != nullptr ) {
//      if( block->block->id() == id ) {
//        return block;
//      }
//    }
//  }

//  return nullptr;
//}

// QNEBlock* SettingsDialog::getBlockWithName( const QString& name ) {
//   const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

//  for( const auto& item : constRefOfList ) {
//    auto* block = qgraphicsitem_cast< QNEBlock* >( item );

//    if( block != nullptr ) {
//      if( block->block->name() == name ) {
//        return block;
//      }
//    }
//  }

//  return nullptr;
//}

void
SettingsDialog::on_pbLoad_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json" );
  QString dir;

  auto* fileDialog = new QFileDialog( this, tr( "Open Saved Config" ), dir, selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;JSON Files (*.json)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect
  // to the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    qDebug() << "QFileDialog::urlSelected QUrl" << fileName << fileName.toDisplayString() << fileName.toLocalFile();

    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( QUrl::fromPercentEncoding( fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

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
  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, [this, fileDialog]( const QString& fileName ) {
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

void
SettingsDialog::loadConfigFromFile( QFile& file ) {
  blocksManager->loadConfigFromFile( file );
}

void
SettingsDialog::on_pbAddBlock_clicked() {
  auto results = ui->twBlocks->selectionModel()->selectedIndexes();

  if( !results.empty() ) {
    auto id = blocksManager->createBlockFromFactory( results.first().data( Qt::UserRole + 1 ).toString(), 0 );

    auto* block = blocksManager->getBlock( id );
    if( block != nullptr ) {
      auto center = ui->gvNodeEditor->mapToScene( ui->gvNodeEditor->viewport()->rect().center() );

      block->positionX = center.x();
      block->positionY = center.y();
    }

    resetAllModels();
  }
}

void
SettingsDialog::on_pbZoomOut_clicked() {
  ui->gvNodeEditor->zoomOut();
}

void
SettingsDialog::on_pbZoomIn_clicked() {
  ui->gvNodeEditor->zoomIn();
}

void
SettingsDialog::on_pbDeleteSelected_clicked() {
  std::vector< int >                       blocks;
  std::vector< BlockConnectionDefinition > connections;

  const auto& constRefOfList = ui->gvNodeEditor->scene()->selectedItems();

  for( auto i : constRefOfList ) {
    auto* connection = qgraphicsitem_cast< QNEConnection* >( i );
    if( connection != nullptr ) {
      connections.emplace_back( connection->port1()->blockPort->idOfBlock,
                                connection->port1()->blockPort->name,
                                connection->port2()->blockPort->idOfBlock,
                                connection->port2()->blockPort->name );
    }

    const auto* block = qgraphicsitem_cast< const QNEBlock* >( i );
    if( block != nullptr ) {
      blocks.emplace_back( block->block->id() );
    }
  }

  blocksManager->deleteConnections( connections );
  blocksManager->deleteBlocks( blocks );

  resetAllModels();
}

void
SettingsDialog::on_gbGrid_toggled( bool arg1 ) {
  saveGridValuesInSettings();
  Q_EMIT setGrid( bool( arg1 ) );
}

void
SettingsDialog::on_dsbGridXStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  Q_EMIT setGridValues( double( ui->dsbGridXStep->value() ),
                        float( ui->dsbGridYStep->value() ),
                        float( ui->dsbGridXStepCoarse->value() ),
                        float( ui->dsbGridYStepCoarse->value() ),
                        float( ui->dsbGridSize->value() ),
                        float( ui->dsbGridCameraThreshold->value() ),
                        float( ui->dsbGridCameraThresholdCoarse->value() ),
                        gridColor,
                        gridColorCoarse );
}

void
SettingsDialog::on_dsbGridYStep_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  Q_EMIT setGridValues( double( ui->dsbGridXStep->value() ),
                        float( ui->dsbGridYStep->value() ),
                        float( ui->dsbGridXStepCoarse->value() ),
                        float( ui->dsbGridYStepCoarse->value() ),
                        float( ui->dsbGridSize->value() ),
                        float( ui->dsbGridCameraThreshold->value() ),
                        float( ui->dsbGridCameraThresholdCoarse->value() ),
                        gridColor,
                        gridColorCoarse );
}

void
SettingsDialog::on_dsbGridSize_valueChanged( double /*arg1*/ ) {
  saveGridValuesInSettings();
  Q_EMIT setGridValues( double( ui->dsbGridXStep->value() ),
                        float( ui->dsbGridYStep->value() ),
                        float( ui->dsbGridXStepCoarse->value() ),
                        float( ui->dsbGridYStepCoarse->value() ),
                        float( ui->dsbGridSize->value() ),
                        float( ui->dsbGridCameraThreshold->value() ),
                        float( ui->dsbGridCameraThresholdCoarse->value() ),
                        gridColor,
                        gridColorCoarse );
}

void
SettingsDialog::on_pbColor_clicked() {
  const QColor color = QColorDialog::getColor( gridColor, this, QStringLiteral( "Select Grid Color" ) );

  if( color.isValid() ) {
    gridColor = color;
    ui->lbColor->setText( gridColor.name() );
    ui->lbColor->setPalette( QPalette( gridColor ) );
    ui->lbColor->setAutoFillBackground( true );

    saveGridValuesInSettings();
    Q_EMIT setGridValues( double( ui->dsbGridXStep->value() ),
                          float( ui->dsbGridYStep->value() ),
                          float( ui->dsbGridXStepCoarse->value() ),
                          float( ui->dsbGridYStepCoarse->value() ),
                          float( ui->dsbGridSize->value() ),
                          float( ui->dsbGridCameraThreshold->value() ),
                          float( ui->dsbGridCameraThresholdCoarse->value() ),
                          gridColor,
                          gridColorCoarse );
  }
}

void
SettingsDialog::saveGridValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

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

void
SettingsDialog::savePathPlannerValuesInSettings() {
  if( !blockSettingsSaving ) {
    QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

    settings.setValue( QStringLiteral( "PathPlanner/PathsInReserve" ), ui->sbPathsInReserve->value() );
    settings.setValue( QStringLiteral( "PathPlanner/MaxDeviation" ), ui->sbGlobalPlannerMaxDeviation->value() );
    settings.sync();
  }
}

void
SettingsDialog::setPathPlannerSettings() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        pathPlannerModel->visible = ui->gbPathPlanner->isChecked();
  //        block->block->setName( ui->lePathPlannerName->text() );
  //        pathPlannerModel->zOffset = ui->dsbPathlPlannerZOffset->value();
  //        pathPlannerModel->viewBox = ui->dsbPathlPlannerViewbox->value();

  //        pathPlannerModel->individualRayColor     = ui->cbPathPlannerRayColor->isChecked();
  //        pathPlannerModel->individualSegmentColor = ui->cbPathPlannerSegmentColor->isChecked();
  //        pathPlannerModel->bisectorsVisible       = ui->cbPathPlannerBisectors->isChecked();

  //        pathPlannerModel->refreshColors();
  //      }
  //    }
  //  }
}

void
SettingsDialog::emitAllConfigSignals() {
  Q_EMIT setGrid( ui->gbGrid->isChecked() );
  emitGridSettings();

  Q_EMIT plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );

  Q_EMIT cameraSmoothingChanged( ui->slCameraSmoothingOrientation->value(), ui->slCameraSmoothingPosition->value() );
}

void
SettingsDialog::on_cbSaveConfigOnExit_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "SaveConfigOnExit" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void
SettingsDialog::on_cbLoadConfigOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "LoadConfigOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void
SettingsDialog::on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "OpenSettingsDialogOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void
SettingsDialog::on_cbRunSimulatorOnStart_stateChanged( int arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "RunSimulatorOnStart" ), bool( arg1 == Qt::CheckState::Checked ) );
  settings.sync();
}

void
SettingsDialog::on_pbDeleteSettings_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.clear();
  settings.sync();
}

void
SettingsDialog::on_pbSaveAsDefault_clicked() {
  saveDefaultConfig();
}

void
SettingsDialog::on_pbLoadSavedConfig_clicked() {
  loadDefaultConfig();
}

void
SettingsDialog::on_pbSetStringToFilename_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    QString dir      = ui->twValues->model()->data( index, Qt::DisplayRole ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, tr( "Set Filename to String" ), dir );

    if( !fileName.isEmpty() ) {
      ui->twValues->model()->setData( index, fileName );
    }
  }

  // as the size of the cell is most likely not enough for the filename, resize it
  ui->twValues->resizeColumnsToContents();
}

void
SettingsDialog::on_pbComPortRefresh_clicked() {
  ui->cbComPorts->clear();

#ifdef SERIALPORT_ENABLED
  const auto infos = QSerialPortInfo::availablePorts();

  for( const QSerialPortInfo& info : infos ) {
    ui->cbComPorts->addItem( info.portName() );
  }

#endif
}

void
SettingsDialog::on_pbBaudrateRefresh_clicked() {
  ui->cbBaudrate->clear();

#ifdef SERIALPORT_ENABLED
  const auto baudrates = QSerialPortInfo::standardBaudRates();

  for( const qint32& baudrate : baudrates ) {
    ui->cbBaudrate->addItem( QString::number( baudrate ) );
  }

#endif
}

void
SettingsDialog::on_pbComPortSet_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    ui->twValues->model()->setData( index, ui->cbComPorts->currentText() );
  }
}

void
SettingsDialog::on_pbBaudrateSet_clicked() {
  QModelIndex index = ui->twValues->currentIndex().siblingAtColumn( 1 );

  if( index.isValid() ) {
    ui->twValues->model()->setData( index, ui->cbBaudrate->currentText() );
  }
}

void
SettingsDialog::on_pbClear_clicked() {
  // select all items, so everything gets deleted with a call to
  // on_pbDeleteSelected_clicked()
  const auto& constRefOfList = ui->gvNodeEditor->scene()->items();

  for( const auto& item : constRefOfList ) {
    item->setSelected( true );
  }

  on_pbDeleteSelected_clicked();
}

void
SettingsDialog::on_cbImplements_currentIndexChanged( int index ) {
  QModelIndex idx       = ui->cbImplements->model()->index( index, 1 );
  QVariant    data      = ui->cbImplements->model()->data( idx );
  auto        implement = qvariant_cast< Implement* >( data );

  implementSectionModel->setDatasource( implement );
  ui->twSections->resizeColumnsToContents();
}

void
SettingsDialog::implementModelReset() {
  if( ui->cbImplements->currentIndex() == -1 || ui->cbImplements->currentIndex() >= ui->cbImplements->model()->rowCount() ) {
    ui->cbImplements->setCurrentIndex( 0 );
  }

  on_cbImplements_currentIndexChanged( ui->cbImplements->currentIndex() );
}

void
SettingsDialog::pathPlannerModelReset() {
  if( ui->cbPathPlanner->currentIndex() == -1 || ui->cbPathPlanner->currentIndex() >= ui->cbPathPlanner->model()->rowCount() ) {
    ui->cbPathPlanner->setCurrentIndex( 0 );
  }

  on_cbPathPlanner_currentIndexChanged( ui->cbPathPlanner->currentIndex() );
}

void
SettingsDialog::resetAllModels() {
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

void
SettingsDialog::resetBlockViewScene() {
  for( const auto& block : blocksManager->blocks() ) {
    bool id0 = false;
    for( const auto& port : block.second->ports() ) {
      if( port.idOfBlock == 0 ) {
        qDebug() << "id0" << block.second->id() << block.second->name() << port.idOfBlock << port.name;
        id0 |= true;
      }
    }
    //    if( id0 ) {
    //      qDebug() << "id0" << block.second->id() << block.second->name();
    //    }
  }

  getSceneOfConfigGraphicsView()->clear();

  for( const auto& block : blocksManager->blocks() ) {
    auto* qneBlock = new QNEBlock( block.second.get() );
    getSceneOfConfigGraphicsView()->addItem( qneBlock );
    qneBlock->depictBlock();
  }

  for( auto* item : getSceneOfConfigGraphicsView()->items() ) {
    QNEBlock* block = qgraphicsitem_cast< QNEBlock* >( item );
    if( block != nullptr ) {
      block->depictConnections();
    }
  }
}

void
SettingsDialog::setSimulatorValues( const double a,
                                    const double b,
                                    const double c,
                                    const double Caf,
                                    const double Car,
                                    const double Cah,
                                    const double m,
                                    const double Iz,
                                    const double sigmaF,
                                    const double sigmaR,
                                    const double sigmaH,
                                    const double Cx,
                                    const double slipX,
                                    const double slewRateAutosteerSteering ) {
  ui->dsbSimGeneralSlewrateAutoSteering->blockSignals( true );
  ui->dsbSimGeneralA->blockSignals( true );
  ui->dsbSimGeneralB->blockSignals( true );
  ui->dsbSimGeneralC->blockSignals( true );
  ui->dsbSimGeneralCaf->blockSignals( true );
  ui->dsbSimGeneralCar->blockSignals( true );
  ui->dsbSimGeneralCah->blockSignals( true );
  ui->dsbSimGeneralM->blockSignals( true );
  ui->dsbSimGeneralIz->blockSignals( true );
  ui->dsbSimGeneralSigmaF->blockSignals( true );
  ui->dsbSimGeneralSigmaR->blockSignals( true );
  ui->dsbSimGeneralSigmaH->blockSignals( true );
  ui->dsbSimGeneralCx->blockSignals( true );
  ui->dsbSimGeneralSlipX->blockSignals( true );

  ui->dsbSimGeneralSlewrateAutoSteering->setValue( slewRateAutosteerSteering );
  ui->dsbSimGeneralA->setValue( a );
  ui->dsbSimGeneralB->setValue( b );
  ui->dsbSimGeneralC->setValue( c );
  ui->dsbSimGeneralCaf->setValue( Caf );
  ui->dsbSimGeneralCar->setValue( Car );
  ui->dsbSimGeneralCah->setValue( Cah );
  ui->dsbSimGeneralM->setValue( m );
  ui->dsbSimGeneralIz->setValue( Iz );
  ui->dsbSimGeneralSigmaF->setValue( sigmaF );
  ui->dsbSimGeneralSigmaR->setValue( sigmaR );
  ui->dsbSimGeneralSigmaH->setValue( sigmaH );
  ui->dsbSimGeneralCx->setValue( Cx );
  ui->dsbSimGeneralSlipX->setValue( slipX * 100 );

  ui->dsbSimGeneralSlewrateAutoSteering->blockSignals( false );
  ui->dsbSimGeneralA->blockSignals( false );
  ui->dsbSimGeneralB->blockSignals( false );
  ui->dsbSimGeneralC->blockSignals( false );
  ui->dsbSimGeneralCaf->blockSignals( false );
  ui->dsbSimGeneralCar->blockSignals( false );
  ui->dsbSimGeneralCah->blockSignals( false );
  ui->dsbSimGeneralM->blockSignals( false );
  ui->dsbSimGeneralIz->blockSignals( false );
  ui->dsbSimGeneralSigmaF->blockSignals( false );
  ui->dsbSimGeneralSigmaR->blockSignals( false );
  ui->dsbSimGeneralSigmaH->blockSignals( false );
  ui->dsbSimGeneralCx->blockSignals( false );
  ui->dsbSimGeneralSlipX->blockSignals( false );
}

void
SettingsDialog::on_btnSectionAdd_clicked() {
  if( !( ui->twSections->selectionModel()->selection().indexes().isEmpty() ) ) {
    ui->twSections->model()->insertRow( ui->twSections->selectionModel()->selection().indexes().constFirst().row() );
  } else {
    ui->twSections->model()->insertRow( ui->twSections->model()->rowCount() );
  }
}

void
SettingsDialog::on_btnSectionRemove_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector< int > rows;

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

void
SettingsDialog::on_pbSetSelectedCellsToNumber_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    ui->twSections->model()->setData( index, ui->dsbSectionsNumber->value() );
  }
}

void
SettingsDialog::on_btnSectionMoveUp_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector< int > rows;

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    rows.push_back( index.row() );
  }

  // forward sort
  std::sort( rows.begin(), rows.end(), std::less<>() );

  auto* implementModel = qobject_cast< ImplementSectionModel* >( ui->twSections->model() );

  if( implementModel != nullptr ) {
    if( rows[0] > 0 ) {
      for( const auto& i : rows ) {
        implementModel->swapElements( i, i - 1 );
      }
    }
  }
}

void
SettingsDialog::on_btnSectionMoveDown_clicked() {
  QItemSelection selection( ui->twSections->selectionModel()->selection() );

  std::vector< int > rows;

  const auto& constRefOfList = selection.indexes();

  for( const auto& index : constRefOfList ) {
    rows.push_back( index.row() );
  }

  // reverse sort
  std::sort( rows.begin(), rows.end(), std::greater<>() );

  auto* implementModel = qobject_cast< ImplementSectionModel* >( ui->twSections->model() );

  if( implementModel != nullptr ) {
    if( ( rows[0] + 1 ) < implementModel->rowCount() ) {
      for( const auto& i : rows ) {
        implementModel->swapElements( i, i + 1 );
      }
    }
  }
}

void
SettingsDialog::on_dsbGridXStepCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void
SettingsDialog::on_dsbGridYStepCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void
SettingsDialog::on_dsbGridCameraThreshold_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void
SettingsDialog::on_dsbGridCameraThresholdCoarse_valueChanged( double ) {
  saveGridValuesInSettings();
  emitGridSettings();
}

void
SettingsDialog::emitGridSettings() {
  Q_EMIT setGridValues( double( ui->dsbGridXStep->value() ),
                        float( ui->dsbGridYStep->value() ),
                        float( ui->dsbGridXStepCoarse->value() ),
                        float( ui->dsbGridYStepCoarse->value() ),
                        float( ui->dsbGridSize->value() ),
                        float( ui->dsbGridCameraThreshold->value() ),
                        float( ui->dsbGridCameraThresholdCoarse->value() ),
                        gridColor,
                        gridColorCoarse );
}

void
SettingsDialog::emitSimulatorValues() {
  Q_EMIT simulatorValuesChanged( ui->dsbSimGeneralA->value(),
                                 ui->dsbSimGeneralB->value(),
                                 ui->dsbSimGeneralC->value(),
                                 ui->dsbSimGeneralCaf->value(),
                                 ui->dsbSimGeneralCar->value(),
                                 ui->dsbSimGeneralCah->value(),
                                 ui->dsbSimGeneralM->value(),
                                 ui->dsbSimGeneralIz->value(),
                                 ui->dsbSimGeneralSigmaF->value(),
                                 ui->dsbSimGeneralSigmaR->value(),
                                 ui->dsbSimGeneralSigmaH->value(),
                                 ui->dsbSimGeneralCx->value(),
                                 ui->dsbSimGeneralSlipX->value() / 100,
                                 ui->dsbSimGeneralSlewrateAutoSteering->value() );
}

void
SettingsDialog::emitNoiseStandartDeviations() {
  Q_EMIT noiseStandartDeviationsChanged( ui->dsbSimNoisePositionXY->value(),
                                         ui->dsbSimNoisePositionZ->value(),
                                         ui->dsbSimNoiseOrientation->value(),
                                         ui->dsbSimNoiseAccelerometer->value(),
                                         ui->dsbSimNoiseGyro->value() );
}

void
SettingsDialog::on_pbColorCoarse_clicked() {
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

void
SettingsDialog::on_sbPathsInReserve_valueChanged( int ) {
  savePathPlannerValuesInSettings();
  Q_EMIT plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );
}

void
SettingsDialog::on_cbRestoreDockPositions_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "RestoreDockPositionsOnStart" ), checked );
  settings.sync();
}

void
SettingsDialog::on_cbSaveDockPositionsOnExit_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "SaveDockPositionsOnExit" ), checked );
  settings.sync();
}

void
SettingsDialog::on_pbMeterDefaults_clicked() {
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

// TODO
void
SettingsDialog::on_rbCrsSimulatorTransverseMercator_toggled( bool checked ) { /*geographicConvertionWrapper->useTM = checked;*/ }

void
SettingsDialog::on_rbCrsGuidanceTransverseMercator_toggled( bool checked ) { /*geographicConvertionWrapper->useTM = checked;*/ }

void
SettingsDialog::on_pbSaveAll_clicked() {
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

void
SettingsDialog::on_pbSaveDockPositionsAsDefault_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );
  KDDockWidgets::LayoutSaver saver;

  settings.setValue( QStringLiteral( "SavedDocks" ), saver.serializeLayout() );
  settings.sync();
}

void
SettingsDialog::on_pbSaveDockPositions_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json" );
  QString dir;
  QString fileName =
    QFileDialog::getSaveFileName( this, tr( "Open Saved Config" ), dir, tr( "All Files (*);;JSON Files (*.json)" ), &selectedFilter );

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

void
SettingsDialog::on_pbLoadDockPositions_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files (*.json" );
  QString dir;

  auto* fileDialog = new QFileDialog( this, tr( "Open Saved Config" ), dir, selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;JSON Files (*.json)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect
  // to the signals instead of using the static functions for the dialogs
  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, [fileDialog]( const QString& fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        QByteArray saveData = loadFile.readAll();

        QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
        QJsonObject   json = loadDoc.object();

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

void
SettingsDialog::on_sbGlobalPlannerMaxDeviation_valueChanged( double ) {
  savePathPlannerValuesInSettings();
  Q_EMIT plannerSettingsChanged( ui->sbPathsInReserve->value(), ui->sbGlobalPlannerMaxDeviation->value() );
}

void
SettingsDialog::on_rbMaterialAuto_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), true );
  settings.setValue( QStringLiteral( "Material/usePhong" ), true );
  settings.sync();
}

void
SettingsDialog::on_rbMaterialPRB_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), true );
  settings.setValue( QStringLiteral( "Material/usePhong" ), false );
  settings.sync();
}

void
SettingsDialog::on_rbMaterialPhong_clicked() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Material/usePBR" ), false );
  settings.setValue( QStringLiteral( "Material/usePhong" ), true );
  settings.sync();
}

void SettingsDialog::on_cbPathPlanner_currentIndexChanged( int index ) {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( index, 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        ui->gbPathPlanner->blockSignals( true );
  //        ui->lePathPlannerName->blockSignals( true );
  //        ui->dsbPathlPlannerZOffset->blockSignals( true );
  //        ui->dsbPathlPlannerViewbox->blockSignals( true );
  //        ui->cbPathPlannerRayColor->blockSignals( true );
  //        ui->cbPathPlannerSegmentColor->blockSignals( true );
  //        ui->cbPathPlannerBisectors->blockSignals( true );

  //        ui->gbPathPlanner->setChecked( pathPlannerModel->visible );
  //        ui->lePathPlannerName->setText( block->block->name() );
  //        ui->dsbPathlPlannerZOffset->setValue( pathPlannerModel->zOffset );
  //        ui->dsbPathlPlannerViewbox->setValue( pathPlannerModel->viewBox );

  //        ui->lbPathPlannerLineColor->setText( pathPlannerModel->linesColor.name() );
  //        ui->lbPathPlannerLineColor->setPalette( pathPlannerModel->linesColor );
  //        ui->lbPathPlannerLineColor->setAutoFillBackground( true );

  //        ui->lbPathPlannerRayColor->setText( pathPlannerModel->raysColor.name() );
  //        ui->lbPathPlannerRayColor->setPalette( pathPlannerModel->raysColor );
  //        ui->lbPathPlannerRayColor->setAutoFillBackground( true );

  //        ui->lbPathPlannerSegmentColor->setText( pathPlannerModel->segmentsColor.name() );
  //        ui->lbPathPlannerSegmentColor->setPalette( pathPlannerModel->segmentsColor );
  //        ui->lbPathPlannerSegmentColor->setAutoFillBackground( true );

  //        ui->lbPathPlannerBisectorsColor->setText( pathPlannerModel->bisectorsColor.name() );
  //        ui->lbPathPlannerBisectorsColor->setPalette( pathPlannerModel->bisectorsColor );
  //        ui->lbPathPlannerBisectorsColor->setAutoFillBackground( true );

  //        ui->cbPathPlannerRayColor->setChecked( pathPlannerModel->individualRayColor );
  //        ui->cbPathPlannerSegmentColor->setChecked( pathPlannerModel->individualSegmentColor );
  //        ui->cbPathPlannerBisectors->setChecked( pathPlannerModel->bisectorsVisible );

  //        ui->gbPathPlanner->blockSignals( false );
  //        ui->lePathPlannerName->blockSignals( false );
  //        ui->dsbPathlPlannerZOffset->blockSignals( false );
  //        ui->dsbPathlPlannerViewbox->blockSignals( false );
  //        ui->cbPathPlannerRayColor->blockSignals( false );
  //        ui->cbPathPlannerSegmentColor->blockSignals( false );
  //        ui->cbPathPlannerBisectors->blockSignals( false );
  //      }
  //    }
  //  }
};

void
SettingsDialog::on_lePathPlannerName_textChanged( const QString& ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_dsbPathlPlannerZOffset_valueChanged( double ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_cbPathPlannerArcColor_stateChanged( int ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_cbPathPlannerRayColor_stateChanged( int ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_cbPathPlannerSegmentColor_stateChanged( int ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_dsbPathlPlannerViewbox_valueChanged( double ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_gbPathPlanner_toggled( bool ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_cbPathPlannerBisectors_stateChanged( int ) {
  setPathPlannerSettings();
}

void
SettingsDialog::on_pbPathPlannerArcColor_clicked() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        const QColor color = QColorDialog::getColor( pathPlannerModel->arcColor, this, QStringLiteral( "Select Arc Color" ) );

  //        if( color.isValid() ) {
  //          pathPlannerModel->arcColor = color;

  //          ui->lbPathPlannerArcColor->setText( color.name() );
  //          ui->lbPathPlannerArcColor->setPalette( color );
  //          ui->lbPathPlannerArcColor->setAutoFillBackground( true );

  //          pathPlannerModel->refreshColors();
  //        }
  //      }
  //    }
  //  }
}

void
SettingsDialog::on_pbPathPlannerLineColor_clicked() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        const QColor color = QColorDialog::getColor( pathPlannerModel->linesColor, this, QStringLiteral( "Select Line Color" ) );

  //        if( color.isValid() ) {
  //          pathPlannerModel->linesColor = color;

  //          ui->lbPathPlannerLineColor->setText( color.name() );
  //          ui->lbPathPlannerLineColor->setPalette( color );
  //          ui->lbPathPlannerLineColor->setAutoFillBackground( true );

  //          pathPlannerModel->refreshColors();
  //        }
  //      }
  //    }
  //  }
}

void
SettingsDialog::on_pbPathPlannerRayColor_clicked() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        const QColor color = QColorDialog::getColor( pathPlannerModel->raysColor, this, QStringLiteral( "Select Ray Color" ) );

  //        if( color.isValid() ) {
  //          pathPlannerModel->raysColor = color;

  //          ui->lbPathPlannerRayColor->setText( color.name() );
  //          ui->lbPathPlannerRayColor->setPalette( color );
  //          ui->lbPathPlannerRayColor->setAutoFillBackground( true );

  //          pathPlannerModel->refreshColors();
  //        }
  //      }
  //    }
  //  }
}

void
SettingsDialog::on_pbPathPlannerSegmentColor_clicked() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        const QColor color = QColorDialog::getColor( pathPlannerModel->segmentsColor, this, QStringLiteral( "Select Segment Color" ) );

  //        if( color.isValid() ) {
  //          pathPlannerModel->segmentsColor = color;

  //          ui->lbPathPlannerSegmentColor->setText( color.name() );
  //          ui->lbPathPlannerSegmentColor->setPalette( color );
  //          ui->lbPathPlannerSegmentColor->setAutoFillBackground( true );

  //          pathPlannerModel->refreshColors();
  //        }
  //      }
  //    }
  //  }
}

void
SettingsDialog::on_pbPathPlannerBisectorsColor_clicked() {
  //  QModelIndex idx  = ui->cbPathPlanner->model()->index( ui->cbPathPlanner->currentIndex(), 1 );
  //  QVariant    data = ui->cbPathPlanner->model()->data( idx );

  //  if( auto* block = qvariant_cast< QNEBlock* >( data ) ) {
  //    for( auto& object : block->objects ) {
  //      if( auto* pathPlannerModel = qobject_cast< PathPlannerModel* >( object ) ) {
  //        const QColor color = QColorDialog::getColor( pathPlannerModel->bisectorsColor, this, QStringLiteral( "Select Bisectors Color" )
  //        );

  //        if( color.isValid() ) {
  //          pathPlannerModel->bisectorsColor = color;

  //          ui->lbPathPlannerBisectorsColor->setText( color.name() );
  //          ui->lbPathPlannerBisectorsColor->setPalette( color );
  //          ui->lbPathPlannerBisectorsColor->setAutoFillBackground( true );

  //          pathPlannerModel->refreshColors();
  //        }
  //      }
  //    }
  //  }
}

void
SettingsDialog::on_dsbGamma_valueChanged( double arg1 ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Qt3D/Gamma" ), arg1 );
  settings.sync();

  qt3dWindow->defaultFrameGraph()->setGamma( float( arg1 ) );
}

void
SettingsDialog::on_cbShowDebugOverlay_toggled( bool checked ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Qt3D/ShowDebugOverlay" ), checked );
  settings.sync();

  qt3dWindow->defaultFrameGraph()->setShowDebugOverlay( checked );
}

void
SettingsDialog::on_pbGammaDefault_clicked() {
  ui->dsbGamma->setValue( 2.2 );
}

void
SettingsDialog::on_pbPlotsDefaults_clicked() {
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

void
SettingsDialog::on_dsbSimNoisePositionXY_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void
SettingsDialog::on_dsbSimNoiseGyro_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void
SettingsDialog::on_dsbSimNoisePositionZ_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void
SettingsDialog::on_dsbSimNoiseOrientation_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void
SettingsDialog::on_dsbSimNoiseAccelerometer_valueChanged( double ) {
  emitNoiseStandartDeviations();
}

void
SettingsDialog::on_slCameraSmoothingOrientation_valueChanged( int value ) {
  Q_EMIT cameraSmoothingChanged( value, ui->slCameraSmoothingPosition->value() );
}

void
SettingsDialog::on_slCameraSmoothingPosition_valueChanged( int value ) {
  Q_EMIT cameraSmoothingChanged( ui->slCameraSmoothingOrientation->value(), value );
}

void
SettingsDialog::on_dsbSimGeneralSlipX_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralA_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralB_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralC_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralSigmaF_valueChanged( double ) {
  emitSimulatorValues();
}
void
SettingsDialog::on_dsbSimGeneralSigmaR_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralSigmaH_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralCaf_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralCar_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralCah_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralM_valueChanged( double ) {
  emitSimulatorValues();
}
void
SettingsDialog::on_dsbSimGeneralIz_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralCx_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_dsbSimGeneralSlewrateAutoSteering_valueChanged( double ) {
  emitSimulatorValues();
}

void
SettingsDialog::on_pbRatePlotDefault_clicked() {
  ui->sbRatePlotDocks->setValue( 30 );
}

void
SettingsDialog::on_pbRateValueDefault_clicked() {
  ui->sbRateValueDocks->setValue( 60 );
}

void
SettingsDialog::on_pbRate3dDefault_clicked() {
  ui->sbRate3dView->setValue( 30 );
}

void
SettingsDialog::on_sbRatePlotDocks_valueChanged( int arg1 ) {
  RateLimiter::setRate( RateLimiter::Type::PlotDock, arg1 );

  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );
  settings.setValue( QStringLiteral( "RateLimit/Plot" ), arg1 );
}

void
SettingsDialog::on_sbRateValueDocks_valueChanged( int arg1 ) {
  RateLimiter::setRate( RateLimiter::Type::ValueDock, arg1 );

  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );
  settings.setValue( QStringLiteral( "RateLimit/Value" ), arg1 );
}

void
SettingsDialog::on_sbRate3dView_valueChanged( int arg1 ) {
  RateLimiter::setRate( RateLimiter::Type::Graphical, arg1 );

  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );
  settings.setValue( QStringLiteral( "RateLimit/Graphics" ), arg1 );
}

void
SettingsDialog::on_twBlocks_doubleClicked( const QModelIndex& index ) {
  on_pbAddBlock_clicked();
}
