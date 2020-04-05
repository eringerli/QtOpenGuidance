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

#include "ActionToolbar.h"
#include "ui_ActionToolbar.h"

#include <QDebug>

ActionToolbar::ActionToolbar( QWidget* parent ) :
  QGroupBox( parent ),
  ui( new Ui::ActionToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  theme = QStringLiteral( "media-playback-start" );
  setTheme( theme );
}

ActionToolbar::~ActionToolbar() {
  delete ui;
}

bool ActionToolbar::state() {
  return ui->pbAction->isChecked();
}

bool ActionToolbar::isCheckable() {
  return ui->pbAction->isCheckable();
}

const QString ActionToolbar::getTheme() {
  return theme;
}

void ActionToolbar::setState( bool state ) {
  if( ui->pbAction->isCheckable() ) {
    ui->pbAction->setChecked( state );
  }
}

void ActionToolbar::setCheckable( bool checkable ) {
  ui->pbAction->setCheckable( checkable );
}

void ActionToolbar::setTheme( const QString theme ) {
  this->theme = theme;
  ui->pbAction->setIcon( QIcon::fromTheme( theme ) );
}

void ActionToolbar::on_pbAutosteer_clicked( bool checked ) {
  emit action( checked );
}
