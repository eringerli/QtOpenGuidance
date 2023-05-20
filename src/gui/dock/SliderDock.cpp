// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SliderDock.h"
#include "ui_SliderDock.h"

#include <cmath>

SliderDock::SliderDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::SliderDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  // some sane defaults
  setDecimals( 1 );
  setMaximum( 10 );
  setMinimum( 0 );
  setDefaultValue( 0 );
  setSliderInverted( false );
}

SliderDock::~SliderDock() { delete ui; }

void
SliderDock::setDecimals( const int decimals ) {
  int tmpDecimals = decimals;

  if( decimals > 2 ) {
    tmpDecimals = 2;
  }

  if( decimals < 0 ) {
    tmpDecimals = 0;
  }

  ui->dsbValue->setDecimals( tmpDecimals );

  double stepSize = std::pow( 10, -tmpDecimals );
  ui->dsbValue->setSingleStep( stepSize );

  ui->slValue->setMaximum( ui->dsbValue->maximum() / stepSize );
  ui->slValue->setMinimum( ui->dsbValue->minimum() / stepSize );
}

void
SliderDock::setMaximum( const double maximum ) {
  ui->dsbValue->setMaximum( maximum );
  ui->slValue->setMaximum( maximum / ui->dsbValue->singleStep() );
}

void
SliderDock::setMinimum( const double minimum ) {
  ui->dsbValue->setMinimum( minimum );
  ui->slValue->setMinimum( minimum / ui->dsbValue->singleStep() );
}

void
SliderDock::setDefaultValue( const double defaultValue ) {
  this->defaultValue = defaultValue;
}

void
SliderDock::setUnit( const QString& unit ) {
  ui->dsbValue->setSuffix( unit );
}

void
SliderDock::setSliderInverted( bool inverted ) {
  ui->slValue->setInvertedControls( inverted );
  ui->slValue->setInvertedAppearance( inverted );
}

double
SliderDock::getValue() const {
  return ui->dsbValue->value();
}

int
SliderDock::getDecimals() const {
  return ui->dsbValue->decimals();
}

double
SliderDock::getMaximum() const {
  return ui->dsbValue->maximum();
}

double
SliderDock::getMinimum() const {
  return ui->dsbValue->minimum();
}

double
SliderDock::getDefaultValue() const {
  return defaultValue;
}

const QString
SliderDock::getUnit() const {
  return ui->dsbValue->suffix();
}

bool
SliderDock::getSliderInverted() const {
  return ui->slValue->invertedControls();
}

void
SliderDock::on_slValue_valueChanged( int value ) {
  double val = double( value ) * ui->dsbValue->singleStep();
  ui->dsbValue->blockSignals( true );
  ui->dsbValue->setValue( val );
  ui->dsbValue->blockSignals( false );
  Q_EMIT valueChanged( val );
}

void
SliderDock::on_dsbValue_valueChanged( double value ) {
  ui->slValue->blockSignals( true );
  ui->slValue->setValue( int( value / ui->dsbValue->singleStep() ) );
  ui->slValue->blockSignals( false );
  Q_EMIT valueChanged( value );
}

void
SliderDock::on_pbValueReset_clicked() {
  ui->dsbValue->setValue( defaultValue );
  ui->slValue->setValue( defaultValue / ui->dsbValue->singleStep() );
  Q_EMIT valueChanged( defaultValue );
}

void
SliderDock::setValue( double value ) {
  ui->dsbValue->setValue( value );
}
