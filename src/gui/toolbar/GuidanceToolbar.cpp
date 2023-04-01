// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QMenu>
#include <QSettings>
#include <QStandardPaths>

#include "GuidanceToolbar.h"
#include "ui_GuidanceToolbar.h"

GuidanceToolbar::GuidanceToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::GuidanceToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  menu = new QMenu( this );

  ui->pbDocks->setMenu( menu );
  ui->pbDocks->setPopupMode( QToolButton::InstantPopup );
}

GuidanceToolbar::~GuidanceToolbar() { delete ui; }

void
GuidanceToolbar::on_cbSimulator_stateChanged( int arg1 ) {
  bool enabled = false;

  if( arg1 == Qt::Checked ) {
    enabled = true;
  }

  Q_EMIT simulatorChanged( enabled );
}

void
GuidanceToolbar::on_pbSettings_clicked() {
  Q_EMIT toggleSettings();
}

void
GuidanceToolbar::cbSimulatorSetChecked( bool enabled ) {
  ui->cbSimulator->setChecked( enabled );
}
