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

#include "SimulatorToolbar.h"
#include "ui_SimulatorToolbar.h"

SimulatorToolbar::SimulatorToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::SimulatorToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

SimulatorToolbar::~SimulatorToolbar() {
  delete ui;
}

void SimulatorToolbar::on_sl_velocity_valueChanged( int value ) {
  double val = double( value ) / 10;
  ui->dsb_velocity->setValue( double( val ) );
  emit velocityChanged( val );
}

void SimulatorToolbar::on_sl_steerangle_valueChanged( int value ) {
  double val = double( value ) / 10;
  ui->dsb_steerangle->setValue( double( val ) );
  emit steerangleChanged( val );
}

void SimulatorToolbar::on_sl_frequency_valueChanged( int value ) {
  ui->dsb_frequency->setValue( double( value ) );
  emit frequencyChanged( value );
}

void SimulatorToolbar::on_dsb_velocity_valueChanged( double val ) {
  ui->sl_velocity->setValue( int( val * 10 ) );
  emit velocityChanged( val );
}

void SimulatorToolbar::on_dsb_steerangle_valueChanged( double val ) {
  ui->sl_steerangle->setValue( int( val * 10 ) );
  emit steerangleChanged( val );
}

void SimulatorToolbar::on_dsb_frequency_valueChanged( double val ) {
  ui->sl_frequency->setValue( int( val ) );
  emit frequencyChanged( int( val ) );
}

void SimulatorToolbar::on_pb_frequencyReset_clicked() {
  ui->dsb_frequency->setValue( 20 );
  ui->sl_frequency->setValue( 20 );
  emit frequencyChanged( 20.0 );
}

void SimulatorToolbar::on_pb_steerangleReset_clicked() {
  ui->dsb_steerangle->setValue( 0 );
  ui->sl_steerangle->setValue( 0 );
  emit steerangleChanged( 0.0 );
}

void SimulatorToolbar::on_pb_velocityReset_clicked() {
  ui->dsb_velocity->setValue( 0 );
  ui->sl_velocity->setValue( 0 );
  emit velocityChanged( 0.0 );
}
