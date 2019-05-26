
#include <QtWidgets>
#include <QObject>

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "../gui/vectorwidget.h"
#include "../gui/lengthwidget.h"

#include "../3d/cameracontroller.h"
#include "../3d/TractorModel.h"
#include "../3d/TrailerModel.h"

#include "../PoseSimulation.h"
#include "../PoseCache.h"

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

  // Factories
  poseCacheFactory = new PoseCacheFactory();
  tractorModelFactory = new TractorModelFactory( rootEntity );
  trailerModelFactory = new TrailerModelFactory( rootEntity );
  fixedKinematicFactory = new FixedKinematicFactory;
  trailerKinematicFactory = new TrailerKinematicFactory;
  vectorFactory = new VectorFactory();
  lengthFactory = new LengthFactory();

  poseCacheFactory->addToCombobox( ui->cbNodeType );
  tractorModelFactory->addToCombobox( ui->cbNodeType );
  trailerModelFactory->addToCombobox( ui->cbNodeType );
  fixedKinematicFactory->addToCombobox( ui->cbNodeType );
  trailerKinematicFactory->addToCombobox( ui->cbNodeType );
  vectorFactory->addToCombobox( ui->cbNodeType );
  lengthFactory->addToCombobox( ui->cbNodeType );
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
}

QGraphicsScene* SettingsDialog::getSceneOfConfigGraphicsView() {
  return ui->gvNodeEditor->scene();
}

void SettingsDialog::toggleVisibility() {
  setVisible( ! isVisible() );
}

void SettingsDialog::emitAntennaPosition() {
  emit antennaPositionChanged( QVector3D( ( float )ui->dsb_antennaX->value(),
                                          ( float )ui->dsb_antennaY->value(),
                                          ( float )ui->dsb_antennaZ->value() ) );
}


void SettingsDialog::on_dsb_antennaX_valueChanged( double /*arg1*/ ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaY_valueChanged( double /*arg1*/ ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaZ_valueChanged( double /*arg1*/ ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_hitchLenght_valueChanged( double arg1 ) {
  emit hitchPositionChanged( QVector3D( -( float )arg1, 0, 0 ) );
}

void SettingsDialog::on_dsb_wheelbase_valueChanged( double arg1 ) {
  emit wheelbaseChanged( ( float )arg1 );
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
  }
}
