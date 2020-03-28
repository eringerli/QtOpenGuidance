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

#include "GuidanceTurning.h"
#include "ui_GuidanceTurning.h"

#include <QAbstractButton>

GuidanceTurning::GuidanceTurning( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::GuidanceTurning ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  ui->pbRight->setText( QString::number( skipRight ) );
  ui->pbLeft->setText( QString::number( skipLeft ) );

  buttonGroupTurn.addButton( ui->pbRight );
  buttonGroupTurn.addButton( ui->pbLeft );
  buttonGroupTurn.setExclusive( false );

  emit numSkipChanged( skipLeft, skipRight );
}

GuidanceTurning::~GuidanceTurning() {
  delete ui;
}

void GuidanceTurning::on_pbLeft_toggled( bool checked ) {
  emit turnLeftToggled( checked );
}

void GuidanceTurning::on_pbRight_toggled( bool checked ) {
  emit turnRightToggled( checked );
}

void GuidanceTurning::on_pbPlusRight_clicked() {
  ++skipRight;
  ui->pbRight->setText( QString::number( skipRight ) );
  emit numSkipChanged( skipLeft, skipRight );
}

void GuidanceTurning::on_pbPlusLeft_clicked() {
  ++skipLeft;
  ui->pbLeft->setText( QString::number( skipLeft ) );
  emit numSkipChanged( skipLeft, skipRight );
}

void GuidanceTurning::on_pbMinusRight_clicked() {
  if( skipRight >= 2 ) {
    --skipRight;
    ui->pbRight->setText( QString::number( skipRight ) );
    emit numSkipChanged( skipLeft, skipRight );
  }
}

void GuidanceTurning::on_pbMinusLeft_clicked() {
  if( skipLeft >= 2 ) {
    --skipLeft;
    ui->pbLeft->setText( QString::number( skipLeft ) );
    emit numSkipChanged( skipLeft, skipRight );
  }
}

void GuidanceTurning::on_pbLeft_pressed() {
  if( ui->pbLeft->isChecked() ) {
    buttonGroupTurn.setExclusive( false );
  } else {
    buttonGroupTurn.setExclusive( true );
  }
}

void GuidanceTurning::on_pbRight_pressed() {
  if( ui->pbRight->isChecked() ) {
    buttonGroupTurn.setExclusive( false );
  } else {
    buttonGroupTurn.setExclusive( true );
  }
}
