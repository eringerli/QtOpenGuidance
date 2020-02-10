// Copyright( C ) 2020 Christian Riggenbach
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
