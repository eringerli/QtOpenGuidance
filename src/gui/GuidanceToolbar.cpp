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

#include <QSettings>
#include <QStandardPaths>

#include "GuidanceToolbar.h"
#include "ui_GuidanceToolbar.h"

GuidanceToolbar::GuidanceToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceToolbar ) {
  ui->setupUi( this );
}

GuidanceToolbar::~GuidanceToolbar() {
  delete ui;
}

void GuidanceToolbar::on_cbSimulator_stateChanged( int arg1 ) {
  bool enabled = false;

  if( arg1 == Qt::Checked ) {
    enabled = true;
  }

  emit simulatorChanged( enabled );
}

void GuidanceToolbar::on_btn_settings_clicked() {
  emit toggleSettings();
}

void GuidanceToolbar::on_cbCamera_stateChanged( int arg1 ) {
  bool enabled = false;

  if( arg1 == Qt::Checked ) {
    enabled = true;
  }

  emit cameraChanged( enabled );
}

void GuidanceToolbar::on_btn_AB_clicked( bool checked ) {
  if( checked ) {
    emit a_clicked();
  } else {
    emit b_clicked();
  }
}

void GuidanceToolbar::on_btn_snap_clicked() {
  emit snap_clicked();
}

void GuidanceToolbar::cbCameraSetChecked( bool enabled ) {
  ui->cbCamera->setChecked( enabled );
}

void GuidanceToolbar::cbSimulatorSetChecked( bool enabled ) {
  ui->cbSimulator->setChecked( enabled );
}

void GuidanceToolbar::on_btn_autosteer_clicked( bool checked ) {
  emit autosteerEnabled( checked );
}
