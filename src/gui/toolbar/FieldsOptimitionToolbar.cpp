// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FieldsOptimitionToolbar.h"
#include "ui_FieldsOptimitionToolbar.h"

FieldsOptimitionToolbar::FieldsOptimitionToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::FieldsOptimitionToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

FieldsOptimitionToolbar::~FieldsOptimitionToolbar() { delete ui; }

void
FieldsOptimitionToolbar::setAlpha( double optimalAlpha, double solidAlpha ) {
  this->optimalAlpha = optimalAlpha;
  this->solidAlpha   = solidAlpha;

  if( ui->cbAlphaShape->currentText() == QLatin1StringView( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( ui->cbAlphaShape->currentText() == QLatin1StringView( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}

void
FieldsOptimitionToolbar::on_pbRecalculate_clicked() {
  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
  Q_EMIT recalculateField();
}

void
FieldsOptimitionToolbar::on_cbAlphaShape_currentTextChanged( const QString& arg1 ) {
  ui->dsbAlpha->setEnabled( arg1 == QLatin1StringView( "Custom" ) );

  if( arg1 == QLatin1StringView( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( arg1 == QLatin1StringView( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }

  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void
FieldsOptimitionToolbar::on_cbConnectEndToStart_stateChanged( int ) {
  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void
FieldsOptimitionToolbar::on_dsbDistanceConnectingPoints_valueChanged( double ) {
  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void
FieldsOptimitionToolbar::on_dsbAlpha_valueChanged( double ) {
  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void
FieldsOptimitionToolbar::on_dsbMaxDeviation_valueChanged( double ) {
  Q_EMIT recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                          ui->dsbAlpha->value(),
                                          ui->dsbMaxDeviation->value(),
                                          ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}
