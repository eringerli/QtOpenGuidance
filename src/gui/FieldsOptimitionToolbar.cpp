#include "FieldsOptimitionToolbar.h"
#include "ui_FieldsOptimitionToolbar.h"

FieldsOptimitionToolbar::FieldsOptimitionToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::FieldsOptimitionToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

FieldsOptimitionToolbar::~FieldsOptimitionToolbar() {
  delete ui;
}

void FieldsOptimitionToolbar::setAlpha( double optimalAlpha, double solidAlpha ) {
  this->optimalAlpha = optimalAlpha;
  this->solidAlpha = solidAlpha;

  if( ui->cbAlphaShape->currentText() == QLatin1String( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( ui->cbAlphaShape->currentText() == QLatin1String( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }
}

void FieldsOptimitionToolbar::on_pbRecalculate_clicked() {
  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
  emit recalculateField();
}

void FieldsOptimitionToolbar::on_cbAlphaShape_currentTextChanged( const QString& arg1 ) {
  ui->dsbAlpha->setEnabled( arg1 == QLatin1String( "Custom" ) );

  if( arg1 == QLatin1String( "Optimal" ) ) {
    ui->dsbAlpha->setValue( optimalAlpha );
  }

  if( arg1 == QLatin1String( "Solid" ) ) {
    ui->dsbAlpha->setValue( solidAlpha );
  }

  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void FieldsOptimitionToolbar::on_cbConnectEndToStart_stateChanged( int ) {

  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void FieldsOptimitionToolbar::on_dsbDistanceConnectingPoints_valueChanged( double ) {
  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void FieldsOptimitionToolbar::on_dsbAlpha_valueChanged( double ) {
  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}

void FieldsOptimitionToolbar::on_dsbMaxDeviation_valueChanged( double ) {
  emit recalculateFieldSettingsChanged( AlphaType( ui->cbAlphaShape->currentIndex() ),
                                        ui->dsbAlpha->value(),
                                        ui->dsbMaxDeviation->value(),
                                        ui->cbConnectEndToStart->isChecked() ? ui->dsbDistanceConnectingPoints->value() : 0 );
}
