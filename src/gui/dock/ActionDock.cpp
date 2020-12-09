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

#include "ActionDock.h"
#include "ui_ActionDock.h"

#include <QDebug>

ActionDock::ActionDock( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::ActionDock ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  theme = QStringLiteral( "media-playback-start" );
  setTheme( theme );
}

ActionDock::~ActionDock() {
  delete ui;
}

bool ActionDock::state() {
  return ui->pbAction->isChecked();
}

bool ActionDock::isCheckable() {
  return ui->pbAction->isCheckable();
}

const QString ActionDock::getTheme() {
  return theme;
}

void ActionDock::setState( const bool state ) {
  if( ui->pbAction->isCheckable() ) {
    ui->pbAction->setChecked( state );
  }
}

void ActionDock::setCheckable( const bool checkable ) {
  ui->pbAction->setCheckable( checkable );
}

void ActionDock::setTheme( const QString& theme ) {
  this->theme = theme;
  ui->pbAction->setIcon( QIcon::fromTheme( theme ) );
}

void ActionDock::on_pbAction_clicked( bool checked ) {
  Q_EMIT action( checked );
}
