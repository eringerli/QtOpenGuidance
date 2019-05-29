#include "CameraToolbar.h"
#include "ui_CameraToolbar.h"

CameraToolbar::CameraToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::CameraToolbar ) {
  ui->setupUi( this );
}

CameraToolbar::~CameraToolbar() {
  delete ui;
}

void CameraToolbar::on_pbUp_clicked() {
  emit tiltUp();
}

void CameraToolbar::on_pbDown_clicked() {
  emit tiltDown();
}

void CameraToolbar::on_pbZoomIn_clicked() {
  emit zoomIn();
}

void CameraToolbar::on_pbZoomOut_clicked() {
  emit zoomOut();
}

void CameraToolbar::on_pbPanLeft_clicked() {
  emit panLeft();
}

void CameraToolbar::on_pbPanRight_clicked() {
  emit panRight();
}

void CameraToolbar::on_pbReset_clicked() {
  emit resetCamera();
}

void CameraToolbar::on_comboBox_currentIndexChanged( int index ) {
  emit setMode( index );
}
