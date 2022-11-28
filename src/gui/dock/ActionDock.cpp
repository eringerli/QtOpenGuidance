// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ActionDock.h"
#include "ui_ActionDock.h"

#include <QDebug>

ActionDock::ActionDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::ActionDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  theme = QStringLiteral( "media-playback-start" );
  setTheme( theme );
}

ActionDock::~ActionDock() { delete ui; }

bool
ActionDock::state() {
  return ui->pbAction->isChecked();
}

bool
ActionDock::isCheckable() {
  return ui->pbAction->isCheckable();
}

const QString
ActionDock::getTheme() {
  return theme;
}

void
ActionDock::setState( const bool state ) {
  if( ui->pbAction->isCheckable() ) {
    ui->pbAction->setChecked( state );
  }
}

void
ActionDock::setCheckable( const bool checkable ) {
  ui->pbAction->setCheckable( checkable );
}

void
ActionDock::setTheme( const QString& theme ) {
  this->theme = theme;
  ui->pbAction->setIcon( QIcon::fromTheme( theme ) );
}

void
ActionDock::on_pbAction_clicked( bool checked ) {
  Q_EMIT action( checked );
}
