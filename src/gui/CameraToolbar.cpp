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

  setContentsMargins( 0, 0, 0, 0 );
}

CameraToolbar::~CameraToolbar() {
  delete ui;
}

void CameraToolbar::on_pbUp_clicked() {
  Q_EMIT tiltUp();
}

void CameraToolbar::on_pbDown_clicked() {
  Q_EMIT tiltDown();
}

void CameraToolbar::on_pbZoomIn_clicked() {
  Q_EMIT zoomIn();
}

void CameraToolbar::on_pbZoomOut_clicked() {
  Q_EMIT zoomOut();
}

void CameraToolbar::on_pbPanLeft_clicked() {
  Q_EMIT panLeft();
}

void CameraToolbar::on_pbPanRight_clicked() {
  Q_EMIT panRight();
}

void CameraToolbar::on_pbReset_clicked() {
  Q_EMIT resetCamera();
}

void CameraToolbar::on_comboBox_currentIndexChanged( int index ) {
  Q_EMIT setMode( index );
}
