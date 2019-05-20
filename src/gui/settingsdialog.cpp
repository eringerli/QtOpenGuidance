#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog( QWidget* parent ) :
  QDialog( parent ),
  ui( new Ui::SettingsDialog ) {
  ui->setupUi( this );
}

SettingsDialog::~SettingsDialog() {
  delete ui;
}

void SettingsDialog::toggleVisibility() {
  setVisible( ! isVisible() );
}

void SettingsDialog::emitAntennaPosition() {
  emit antennaPositionChanged( QVector3D( ui->dsb_antennaX->value(),
                                          ui->dsb_antennaY->value(),
                                          ui->dsb_antennaZ->value() ) );
}


void SettingsDialog::on_dsb_antennaX_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaY_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_antennaZ_valueChanged( double arg1 ) {
  emitAntennaPosition();
}

void SettingsDialog::on_dsb_hitchLenght_valueChanged( double arg1 ) {
  emit hitchPositionChanged( QVector3D(-arg1,0,0) );
}

void SettingsDialog::on_dsb_wheelbase_valueChanged( double arg1 ) {
  emit wheelbaseChanged( arg1 );
}

void SettingsDialog::on_rb_fixedCamera_clicked() {
  emit cameraChanged( 0 );
}

void SettingsDialog::on_rb_firstPersonCamera_clicked() {
  emit cameraChanged( 1 );
}

void SettingsDialog::on_rb_orbitCamera_clicked() {
  emit cameraChanged( 2 );
}
