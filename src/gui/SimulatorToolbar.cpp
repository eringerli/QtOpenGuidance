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

#include "SimulatorToolbar.h"
#include "ui_SimulatorToolbar.h"

SimulatorToolbar::SimulatorToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::SimulatorToolbar ) {
  ui->setupUi( this );
}

SimulatorToolbar::~SimulatorToolbar() {
  delete ui;
}

void SimulatorToolbar::on_sl_velocity_valueChanged( int value ) {
  emit velocityChanged( ( float )value / 10 );
}

void SimulatorToolbar::on_sl_steerangle_valueChanged( int value ) {
  emit steerangleChanged( ( float )value / 10 );
}

void SimulatorToolbar::on_sl_frequency_valueChanged( int value ) {
  emit frequencyChanged( value );
}
