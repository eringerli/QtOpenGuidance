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

#include <QtWidgets>
#include <QObject>

#include <QFileDialog>

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include "../block/VectorObject.h"
#include "../block/LengthObject.h"

#include "../block/CameraController.h"
#include "../block/TractorModel.h"
#include "../block/TrailerModel.h"

#include "../block/PoseSimulation.h"
#include "../block/PoseCache.h"
#include "../block/DebugSink.h"

#include "../kinematic/FixedKinematic.h"
#include "../kinematic/TrailerKinematic.h"

SettingsDialog::SettingsDialog( Qt3DCore::QEntity* rootEntity, QWidget* parent ) :
  QDialog( parent ),
  ui( new Ui::SettingsDialog ),
  rootEntity( rootEntity ) {
  ui->setupUi( this );

  ui->gvNodeEditor->setDragMode( QGraphicsView::RubberBandDrag );

  QGraphicsScene* scene = new QGraphicsScene();
  ui->gvNodeEditor->setScene( scene );

  ui->gvNodeEditor->setRenderHint( QPainter::Antialiasing, true );

  QNodesEditor* nodesEditor = new QNodesEditor( this );
  nodesEditor->install( scene );

  // Models for the tableview
  vectorBlockModel = new VectorBlockModel( scene );
  lengthBlockModel = new LengthBlockModel( scene );
  vectorBlockModel->addToCombobox( ui->cbValues );
  lengthBlockModel->addToCombobox( ui->cbValues );

  // Factories for the blocks
  poseCacheFactory = new PoseCacheFactory();
  tractorModelFactory = new TractorModelFactory( rootEntity );
  trailerModelFactory = new TrailerModelFactory( rootEntity );
  fixedKinematicFactory = new FixedKinematicFactory;
  trailerKinematicFactory = new TrailerKinematicFactory;
  vectorFactory = new VectorFactory();
  lengthFactory = new LengthFactory();
  debugSinkFactory = new DebugSinkFactory();

  poseCacheFactory->addToCombobox( ui->cbNodeType );
  tractorModelFactory->addToCombobox( ui->cbNodeType );
  trailerModelFactory->addToCombobox( ui->cbNodeType );
  fixedKinematicFactory->addToCombobox( ui->cbNodeType );
  trailerKinematicFactory->addToCombobox( ui->cbNodeType );
  vectorFactory->addToCombobox( ui->cbNodeType );
  lengthFactory->addToCombobox( ui->cbNodeType );
  debugSinkFactory->addToCombobox( ui->cbNodeType );
}

SettingsDialog::~SettingsDialog() {
  delete ui;

  delete poseCacheFactory;
  delete tractorModelFactory;
  delete trailerModelFactory;
  delete fixedKinematicFactory;
  delete trailerKinematicFactory;
  delete vectorFactory;
  delete lengthFactory;
  delete debugSinkFactory;

  delete vectorBlockModel;
  delete lengthBlockModel;
}

QGraphicsScene* SettingsDialog::getSceneOfConfigGraphicsView() {
  return ui->gvNodeEditor->scene();
}

void SettingsDialog::toggleVisibility() {
  setVisible( ! isVisible() );
}

void SettingsDialog::on_cbValues_currentIndexChanged( int /*index*/ ) {
  QAbstractTableModel* model = qobject_cast<QAbstractTableModel*>( qvariant_cast<QAbstractTableModel*>( ui->cbValues->currentData() ) );

  if( model ) {
    QItemSelectionModel* m = ui->twValues->selectionModel();
    ui->twValues->setModel( model );
    delete m;
  }
}

void SettingsDialog::on_pbSaveSelected_clicked() {
  QString selectedFilter = QStringLiteral( "JSON Files" );
  QString dir;
  QString fileName = QFileDialog::getSaveFileName( this,
                     tr( "Open Saved Config" ),
                     dir,
                     tr( "All Files (*);;JSON Files (*.json)" ),
                     &selectedFilter );

  if( !fileName.isEmpty() ) {

    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning( "Couldn't open save file." );
      return;
    }

    QJsonObject jsonObject;
    QJsonArray blocks;
    QJsonArray connections;
    jsonObject["blocks"] = blocks;
    jsonObject["connections"] = connections;

    foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
      {
        {
          QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

          if( block ) {
            block->toJSON( jsonObject );
          }
        }

        {
          QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

          if( connection ) {
            connection->toJSON( jsonObject );
          }
        }
      }
    }

    QJsonDocument jsonDocument( jsonObject );
    QByteArray bytes = jsonDocument.toJson();
    qDebug() << bytes;

    saveFile.write( jsonDocument.toJson() );
  }
}

void SettingsDialog::on_pbLoad_clicked() {

}

void SettingsDialog::on_pbAddBlock_clicked() {
  QString currentText = ui->cbNodeType->currentText();
  GuidanceFactory* factory = qobject_cast<GuidanceFactory*>( qvariant_cast<GuidanceFactory*>( ui->cbNodeType->currentData() ) );

  if( factory ) {
    GuidanceBase* obj = factory->createNewObject();
    factory->createBlock( ui->gvNodeEditor->scene(), obj );

    // reset the models
    if( qobject_cast<VectorObject*>( obj ) ) {
      vectorBlockModel->resetModel();
    }
  }
}

void SettingsDialog::on_pbZoomOut_clicked() {
  ui->gvNodeEditor->zoomOut();
}

void SettingsDialog::on_pbZoomIn_clicked() {
  ui->gvNodeEditor->zoomIn();
}

void SettingsDialog::on_pbDeleteSelected_clicked() {
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    {
      QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block && !block->systemBlock ) {
        delete block;
        return;
      }
    }

    {
      QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

      if( connection ) {
        delete connection;
        return;
      }
    }
  }
}
