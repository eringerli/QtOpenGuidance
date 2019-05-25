
#include <QtWidgets>

#include "qneblock.h"
#include "qneconnection.h"
#include "qneport.h"
#include "qnodeseditor.h"

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

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
}

SettingsDialog::~SettingsDialog() {
  delete ui;
}

QGraphicsScene* SettingsDialog::getSceneOfConfigGraphicsView() {
  return ui->gvNodeEditor->scene();
}

void SettingsDialog::toggleVisibility() {
  setVisible( ! isVisible() );
}

void SettingsDialog::emitAntennaPosition() {
  emit antennaPositionChanged( QVector3D( ui->dsb_antennaX->value(),
                                          ui->dsb_antennaY->value(),
                                          ui->dsb_antennaZ->value() ) );
}


void SettingsDialog::on_dsb_antennaX_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaY_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaZ_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_hitchLenght_valueChanged( double arg1 ) {
  emit hitchPositionChanged( QVector3D( -arg1, 0, 0 ) );
}

void SettingsDialog::on_dsb_wheelbase_valueChanged( double arg1 ) {
  emit wheelbaseChanged( arg1 );
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
  while( !ui->gvNodeEditor->scene()->selectedItems().isEmpty() ) {
    delete ui->gvNodeEditor->scene()->selectedItems().front();
  }
}

void SettingsDialog::on_pushButton_clicked() {
  QString currentText = ui->cbNodeType->currentText();

  if( currentText == "Pose Cache" ) {
    PoseCache* obj = new PoseCache;
    QNEBlock* b = new QNEBlock( obj );
    ui->gvNodeEditor->scene()->addItem( b );
    b->addPort( "Cache", "", 0, QNEPort::NamePort );
    b->addPort( "PoseCache", "", 0, QNEPort::TypePort );
    b->addInputPort( "Position", SLOT( setPosition( QVector3D ) ) );
    b->addInputPort( "Orientation", SLOT( setOrientation( QQuaternion ) ) );
    b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
    b->addOutputPort( "Pose", SIGNAL( poseChanged( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Steering Angle", SIGNAL( steeringAngleChanged( float ) ) );
  }

  if( currentText == "Fixed Kinematics" ) {
    FixedKinematic* obj = new FixedKinematic;
    obj->setOffsetHookPointPosition( QVector3D( 3, 0, 1.5 ) );
    obj->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );

    QNEBlock* b = new QNEBlock( obj );
    ui->gvNodeEditor->scene()->addItem( b );
    b->addPort( "Fixed", "", 0, QNEPort::NamePort );
    b->addPort( "Fixed Kinematics", "", 0, QNEPort::TypePort );
    b->addInputPort( "Pose", SLOT( setPose( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ) );
  }

  if( currentText == "Trailer Kinematics" ) {
    TrailerKinematic* obj = new TrailerKinematic;
    obj->setOffsetHookPointPosition( QVector3D( 6, 0, 0 ) );
    obj->setOffsetTowPointPosition( QVector3D( -1, 0, 0 ) );

    QNEBlock* b = new QNEBlock( obj );
    ui->gvNodeEditor->scene()->addItem( b );
    b->addPort( "Trailer", "", 0, QNEPort::NamePort );
    b->addPort( "Trailer Kinematics", "", 0, QNEPort::TypePort );
    b->addInputPort( "Pose", SLOT( setPose( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ) );
    b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ) );
  }

  if( currentText == "Tractor Model" ) {
    TractorModel* obj = new TractorModel( rootEntity );

    QNEBlock* b = new QNEBlock( obj );
    ui->gvNodeEditor->scene()->addItem( b );
    b->addPort( "Tractor", "", 0, QNEPort::NamePort );
    b->addPort( "Tractor Model", "", 0, QNEPort::TypePort );
    b->addInputPort( "Pose Hook Point", SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );
    b->addInputPort( "Pose Pivot Point", SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
    b->addInputPort( "Pose Tow Point", SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
    b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
  }

  if( currentText == "Trailer Model" ) {
    TrailerModel* obj = new TrailerModel( rootEntity );
    obj->setOffsetHookPointPosition( QVector3D( 6, 0, 0 ) );

    QNEBlock* b = new QNEBlock( obj );
    ui->gvNodeEditor->scene()->addItem( b );
    b->addPort( "Trailer", "", 0, QNEPort::NamePort );
    b->addPort( "Trailer Model", "", 0, QNEPort::TypePort );
    b->addInputPort( "Pose Hook Point", SLOT( setPoseHookPoint( QVector3D, QQuaternion ) ) );
    b->addInputPort( "Pose Pivot Point", SLOT( setPosePivotPoint( QVector3D, QQuaternion ) ) );
    b->addInputPort( "Pose Tow Point", SLOT( setPoseTowPoint( QVector3D, QQuaternion ) ) );
  }

}
