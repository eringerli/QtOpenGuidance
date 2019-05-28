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

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "../gui/vectorobject.h"
#include "../gui/lengthobject.h"

#include "../3d/cameracontroller.h"
#include "../3d/TractorModel.h"
#include "../3d/TrailerModel.h"

#include "../PoseSimulation.h"
#include "../PoseCache.h"
#include "../DebugSink.h"

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

void SettingsDialog::on_rb_fixedCamera_clicked() {
  emit cameraChanged( 0 );
}

void SettingsDialog::on_rb_firstPersonCamera_clicked() {
  emit cameraChanged( 1 );
}

void SettingsDialog::on_rb_orbitCamera_clicked() {
  emit cameraChanged( 2 );
}

void SettingsDialog::on_pushButton_3_clicked() {
  ui->gvNodeEditor->zoomIn();
}

void SettingsDialog::on_pushButton_2_clicked() {
  ui->gvNodeEditor->zoomOut();
}

void SettingsDialog::on_pushButton_4_clicked() {
  foreach( QGraphicsItem* item, ui->gvNodeEditor->scene()->selectedItems() ) {
    QNEBlock* block = qgraphicsitem_cast<QNEBlock*>( item );

    if( block && block->deleteable ) {
      delete block;
    }
  }
}

void SettingsDialog::on_pushButton_clicked() {
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

void SettingsDialog::on_cbValues_currentIndexChanged( int /*index*/ ) {
  QAbstractTableModel* model = qobject_cast<QAbstractTableModel*>( qvariant_cast<QAbstractTableModel*>( ui->cbValues->currentData() ) );

  if( model ) {
    QItemSelectionModel* m = ui->twValues->selectionModel();
    ui->twValues->setModel( model );
    delete m;
  }
}
