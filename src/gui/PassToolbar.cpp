#include "PassToolbar.h"
#include "ui_PassToolbar.h"

PassToolbar::PassToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::PassToolbar ) {
  ui->setupUi( this );
}

PassToolbar::~PassToolbar() {
  delete ui;
}

void PassToolbar::emitPassSettings() {
  emit passSettingsChanged(
    ui->sbForwardPasses->value(),
    ui->sbReversePasses->value(),
    ui->rbStartRight->isChecked(),
    ui->cbMirror->isChecked() );
}

void PassToolbar::on_sbForwardPasses_valueChanged( int ) {
  emitPassSettings();
}

void PassToolbar::on_sbReversePasses_valueChanged( int ) {
  emitPassSettings();
}

void PassToolbar::on_rbStartRight_toggled( bool ) {
  emitPassSettings();
}

void PassToolbar::on_rbStartLeft_toggled( bool ) {
  emitPassSettings();
}

void PassToolbar::on_pbResetForwardPasses_clicked() {
  ui->sbForwardPasses->setValue( 0 );
}

void PassToolbar::on_pbResetReversePasses_clicked() {
  ui->sbReversePasses->setValue( 0 );
}

void PassToolbar::on_pbSetReverseToForwardPasses_clicked() {
  ui->sbReversePasses->setValue( ui->sbForwardPasses->value() );
}

void PassToolbar::on_pbResetPassNumber_clicked() {
  ui->sbPassNumber->setValue( 0 );
}

void PassToolbar::on_pbSetPassNumber_clicked() {
  emit passNumberChanged( ui->sbPassNumber->value() );
}

void PassToolbar::on_cbMirror_stateChanged( int ) {
  emitPassSettings();
}