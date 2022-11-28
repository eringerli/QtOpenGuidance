// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GlobalPlannerToolbar.h"
#include "ui_GlobalPlannerToolbar.h"

GlobalPlannerToolbar::GlobalPlannerToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::GlobalPlannerToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );
}

GlobalPlannerToolbar::~GlobalPlannerToolbar() { delete ui; }

void
GlobalPlannerToolbar::on_pbAB_clicked( bool checked ) {
  if( checked ) {
    ui->pbAB->setText( QStringLiteral( "B" ) );
    Q_EMIT setAPoint();
  } else {
    if( ui->pbAB->text() == QStringLiteral( "B" ) ) {
      ui->pbAB->setText( QStringLiteral( "+" ) );
      ui->pbAB->setCheckable( false );
      Q_EMIT setBPoint();
    } else {
      Q_EMIT setAdditionalPoint();
    }
  }
}

void
GlobalPlannerToolbar::on_pbSnap_clicked() {
  Q_EMIT snap();
}

void
GlobalPlannerToolbar::resetToolbar() {
  ui->pbAB->setText( QStringLiteral( "A" ) );
  ui->pbAB->setCheckable( true );
}

void
GlobalPlannerToolbar::toolbarToAdditionalPointSet() {
  ui->pbAB->setText( QStringLiteral( "+" ) );
  ui->pbAB->setCheckable( false );
}

void
GlobalPlannerToolbar::on_pbAbContinuous_clicked( bool checked ) {
  Q_EMIT setAdditionalPointsContinous( checked );
}
