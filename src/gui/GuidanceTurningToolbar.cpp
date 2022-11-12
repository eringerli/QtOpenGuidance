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

#include "GuidanceTurningToolbar.h"
#include "ui_GuidanceTurningToolbar.h"

#include <QAbstractButton>

GuidanceTurningToolbar::GuidanceTurningToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::GuidanceTurningToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  ui->pbRight->setText( QString::number( skipRight ) );
  ui->pbLeft->setText( QString::number( skipLeft ) );

  buttonGroupTurn.addButton( ui->pbRight );
  buttonGroupTurn.addButton( ui->pbLeft );
  buttonGroupTurn.setExclusive( false );

  Q_EMIT numSkipChanged( skipLeft, skipRight );
}

GuidanceTurningToolbar::~GuidanceTurningToolbar() { delete ui; }

void
GuidanceTurningToolbar::on_pbLeft_toggled( bool checked ) {
  Q_EMIT turnLeftToggled( checked );
}

void
GuidanceTurningToolbar::on_pbRight_toggled( bool checked ) {
  Q_EMIT turnRightToggled( checked );
}

void
GuidanceTurningToolbar::on_pbPlusRight_clicked() {
  ++skipRight;
  ui->pbRight->setText( QString::number( skipRight ) );
  Q_EMIT numSkipChanged( skipLeft, skipRight );
}

void
GuidanceTurningToolbar::on_pbPlusLeft_clicked() {
  ++skipLeft;
  ui->pbLeft->setText( QString::number( skipLeft ) );
  Q_EMIT numSkipChanged( skipLeft, skipRight );
}

void
GuidanceTurningToolbar::on_pbMinusRight_clicked() {
  if( skipRight >= 2 ) {
    --skipRight;
    ui->pbRight->setText( QString::number( skipRight ) );
    Q_EMIT numSkipChanged( skipLeft, skipRight );
  }
}

void
GuidanceTurningToolbar::on_pbMinusLeft_clicked() {
  if( skipLeft >= 2 ) {
    --skipLeft;
    ui->pbLeft->setText( QString::number( skipLeft ) );
    Q_EMIT numSkipChanged( skipLeft, skipRight );
  }
}

void
GuidanceTurningToolbar::on_pbLeft_pressed() {
  if( ui->pbLeft->isChecked() ) {
    buttonGroupTurn.setExclusive( false );
  } else {
    buttonGroupTurn.setExclusive( true );
  }
}

void
GuidanceTurningToolbar::on_pbRight_pressed() {
  if( ui->pbRight->isChecked() ) {
    buttonGroupTurn.setExclusive( false );
  } else {
    buttonGroupTurn.setExclusive( true );
  }
}

void
GuidanceTurningToolbar::resetTurningState() {
  buttonGroupTurn.setExclusive( false );
  ui->pbLeft->setChecked( false );
  ui->pbRight->setChecked( false );
}
