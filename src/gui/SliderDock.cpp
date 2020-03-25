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

#include "SliderDock.h"
#include "ui_SliderDock.h"

SliderDock::SliderDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::SliderDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  // some sane defaults
  setDecimals( 1 );
  setMaximum( 10 );
  setMinimum( 0 );
  setDefaultValue( 0 );
  setSliderInverted( false );
}

SliderDock::~SliderDock() {
  delete ui;
}

void SliderDock::setDecimals( const int decimals ) {
  int tmpDecimals = decimals;

  if( decimals > 2 ) {
    tmpDecimals = 2;
  }

  if( decimals < 0 ) {
    tmpDecimals = 0;
  }

  ui->dsbValue->setDecimals( tmpDecimals );

  double stepSize = /*10 **/ pow( 10, -tmpDecimals );
  ui->dsbValue->setSingleStep( stepSize );

  ui->slValue->setMaximum( ui->dsbValue->maximum() / stepSize );
  ui->slValue->setMinimum( ui->dsbValue->minimum() / stepSize );

}

void SliderDock::setMaximum( const double maximum ) {
  ui->dsbValue->setMaximum( maximum );
  ui->slValue->setMaximum( maximum / ui->dsbValue->singleStep() );
}

void SliderDock::setMinimum( const double minimum ) {
  ui->dsbValue->setMinimum( minimum );
  ui->slValue->setMinimum( minimum / ui->dsbValue->singleStep() );
}

void SliderDock::setDefaultValue( const double defaultValue ) {
  this->defaultValue = defaultValue;
}

void SliderDock::setUnit( const QString& unit ) {
  ui->dsbValue->setSuffix( unit );
}

void SliderDock::setSliderInverted( bool inverted ) {
  ui->slValue->setInvertedControls( inverted );
  ui->slValue->setInvertedAppearance( inverted );
}

double SliderDock::getValue() {
  return ui->dsbValue->value();
}

int SliderDock::getDecimals() {
  return ui->dsbValue->decimals();
}

double SliderDock::getMaximum() {
  return ui->dsbValue->maximum();
}

double SliderDock::getMinimum() {
  return ui->dsbValue->minimum();
}

double SliderDock::getDefaultValue() {
  return defaultValue;
}

const QString SliderDock::getUnit() {
  return ui->dsbValue->suffix();
}

bool SliderDock::getSliderInverted() {
  return ui->slValue->invertedControls();
}

void SliderDock::on_slValue_valueChanged( int value ) {
  double val = double( value ) * ui->dsbValue->singleStep();
  ui->dsbValue->setValue( val );
  emit valueChanged( val );
}

void SliderDock::on_dsbValue_valueChanged( double value ) {
  ui->slValue->setValue( int( value / ui->dsbValue->singleStep() ) );
  emit valueChanged( value );
}

void SliderDock::on_pbValueReset_clicked() {
  ui->dsbValue->setValue( defaultValue );
  ui->slValue->setValue( defaultValue / ui->dsbValue->singleStep() );
  emit valueChanged( defaultValue );
}

void SliderDock::setValue( double value ) {
  on_dsbValue_valueChanged( value );
  emit valueChanged( value );
}
