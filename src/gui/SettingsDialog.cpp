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
  filterModel = new QSortFilterProxyModel( scene );
  vectorBlockModel = new VectorBlockModel( scene );
  lengthBlockModel = new LengthBlockModel( scene );
  vectorBlockModel->addToCombobox( ui->cbValues );
  lengthBlockModel->addToCombobox( ui->cbValues );
  filterModel->setSourceModel( vectorBlockModel );
  filterModel->sort( 0, Qt::AscendingOrder );
  ui->twValues->setModel( filterModel );

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
    filterModel->setSourceModel( model );
    ui->twValues->resizeColumnsToContents();
  }
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

    QJsonDocument jsonDocument( jsonObject );
    saveFile.write( jsonDocument.toJson() );
  }
}

QNEBlock* SettingsDialog::getBlockWithId( int id ) {
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->items() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block ) {
      if( block->m_id == id ) {
        return block;
      }
    }
  }

  return nullptr;
}

void SettingsDialog::on_pbLoad_clicked() {

  QString selectedFilter = QStringLiteral( "JSON Files (*.json)" );
  QString dir;
  QString fileName = QFileDialog::getOpenFileName( this,
                     tr( "Open Saved Config" ),
                     dir,
                     tr( "All Files (*);;JSON Files (*.json)" ),
                     &selectedFilter );

  if( !fileName.isEmpty() ) {
    QFile loadFile( fileName );

    if( !loadFile.open( QIODevice::ReadOnly ) ) {
      qWarning( "Couldn't open save file." );
      return;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
    QJsonObject json = loadDoc.object();

    // as the new object get new id, here is a QMap to hold the conversions
    // first int: id in file, second int: id in the graphicsview
    QMap<int, int> idMap;

    if( json.contains( "blocks" ) && json["blocks"].isArray() ) {
      QJsonArray blocksArray = json["blocks"].toArray();

      for( int blockIndex = 0; blockIndex < blocksArray.size(); ++blockIndex ) {
        QJsonObject blockObject = blocksArray[blockIndex].toObject();
        int id = blockObject["id"].toInt( 0 );

        // if id is a system-id -> search the block and set the values
        if( id != 0 ) {
          // system id -> don't create new blocks
          if( id < int( QNEBlock::IdRange::UserIdStart ) ) {
            QNEBlock* block = getBlockWithId( id );

            if( block ) {
              idMap.insert( id, block->m_id );
              block->setX( blockObject["positionX"].toDouble( 0 ) );
              block->setY( blockObject["positionY"].toDouble( 0 ) );
            }

            // id is not a system-id -> create new blocks
          } else {
            int index = ui->cbNodeType->findText( blockObject["type"].toString(), Qt::MatchExactly );
            GuidanceFactory* factory = qobject_cast<GuidanceFactory*>( qvariant_cast<QObject*>( ui->cbNodeType->itemData( index ) ) );

            if( factory ) {
              GuidanceBase* obj = factory->createNewObject();
              QNEBlock* block = factory->createBlock( ui->gvNodeEditor->scene(), obj );

              idMap.insert( id, block->m_id );

              block->setX( blockObject["positionX"].toDouble( 0 ) );
              block->setY( blockObject["positionY"].toDouble( 0 ) );
              block->setName( blockObject["name"].toString( factory->getNameOfFactory() ) );
              block->fromJSON( blockObject );
              block->setSelected( true );

              // reset the models
              if( qobject_cast<VectorObject*>( obj ) ) {
                vectorBlockModel->resetModel();
              }
            }
          }

        }
      }
    }

    if( json.contains( "connections" ) && json["connections"].isArray() ) {
      QJsonArray connectionsArray = json["connections"].toArray();

      for( int connectionsIndex = 0; connectionsIndex < connectionsArray.size(); ++connectionsIndex ) {
        QJsonObject connectionsObject = connectionsArray[connectionsIndex].toObject();

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
                QNEConnection* conn = new QNEConnection();
                blockFrom->scene()->addItem( conn );
                conn->setPort1( portFrom );

                if( conn->setPort2( portTo ) ) {
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
      QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

      if( block ) {
        block->emitConfigSignals();
      }
    }

    // rescale the tableview
    ui->twValues->resizeColumnsToContents();
  }
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
    QNEConnection* connection = qgraphicsitem_cast<QNEConnection*>( item );

    if( connection != nullptr ) {
      delete connection;
    }
  }

  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block != nullptr ) {
      if( !block->systemBlock ) {
        delete block;
      }
    }
  }
}
