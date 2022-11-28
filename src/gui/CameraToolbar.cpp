// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CameraToolbar.h"
#include "ui_CameraToolbar.h"

CameraToolbar::CameraToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::CameraToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

CameraToolbar::~CameraToolbar() { delete ui; }

void
CameraToolbar::on_pbUp_clicked() {
  Q_EMIT tiltUp();
}

void
CameraToolbar::on_pbDown_clicked() {
  Q_EMIT tiltDown();
}

void
CameraToolbar::on_pbZoomIn_clicked() {
  Q_EMIT zoomIn();
}

void
CameraToolbar::on_pbZoomOut_clicked() {
  Q_EMIT zoomOut();
}

void
CameraToolbar::on_pbPanLeft_clicked() {
  Q_EMIT panLeft();
}

void
CameraToolbar::on_pbPanRight_clicked() {
  Q_EMIT panRight();
}

void
CameraToolbar::on_pbReset_clicked() {
  Q_EMIT resetCamera();
}

void
CameraToolbar::on_comboBox_currentIndexChanged( int index ) {
  Q_EMIT setMode( index );
}
