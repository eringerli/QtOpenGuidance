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

#include "PassToolbar.h"
#include "ui_PassToolbar.h"

PassToolbar::PassToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::PassToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

PassToolbar::~PassToolbar() { delete ui; }

void
PassToolbar::emitPassSettings() {
  Q_EMIT passSettingsChanged(
    ui->sbForwardPasses->value(), ui->sbReversePasses->value(), ui->rbStartRight->isChecked(), ui->cbMirror->isChecked() );
}

void
PassToolbar::on_sbForwardPasses_valueChanged( int ) {
  emitPassSettings();
}

void
PassToolbar::on_sbReversePasses_valueChanged( int ) {
  emitPassSettings();
}

void
PassToolbar::on_rbStartRight_toggled( bool ) {
  emitPassSettings();
}

void
PassToolbar::on_rbStartLeft_toggled( bool ) {
  emitPassSettings();
}

void
PassToolbar::on_pbResetForwardPasses_clicked() {
  ui->sbForwardPasses->setValue( 0 );
}

void
PassToolbar::on_pbResetReversePasses_clicked() {
  ui->sbReversePasses->setValue( 0 );
}

void
PassToolbar::on_pbSetReverseToForwardPasses_clicked() {
  ui->sbReversePasses->setValue( ui->sbForwardPasses->value() );
}

void
PassToolbar::on_pbResetPassNumber_clicked() {
  ui->sbPassNumber->setValue( 0 );
}

void
PassToolbar::on_pbSetPassNumber_clicked() {
  Q_EMIT passNumberChanged( ui->sbPassNumber->value() );
}

void
PassToolbar::on_cbMirror_stateChanged( int ) {
  emitPassSettings();
}
