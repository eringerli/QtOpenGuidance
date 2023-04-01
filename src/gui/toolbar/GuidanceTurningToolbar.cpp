// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
