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

#include "NewOpenSaveToolbar.h"
#include "ui_NewOpenSaveToolbar.h"

#include <QMenu>

NewOpenSaveToolbar::NewOpenSaveToolbar( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::NewOpenSaveToolbar ) {
  ui->setupUi( this );

  setContentsMargins( 0, 0, 0, 0 );

  newMenu  = new QMenu( this );
  openMenu = new QMenu( this );
  saveMenu = new QMenu( this );

  ui->pbNew->setMenu( newMenu );
  ui->pbNew->setPopupMode( QToolButton::InstantPopup );
  ui->pbOpen->setMenu( openMenu );
  ui->pbOpen->setPopupMode( QToolButton::InstantPopup );
  ui->pbSave->setMenu( saveMenu );
  ui->pbSave->setPopupMode( QToolButton::InstantPopup );
}

NewOpenSaveToolbar::~NewOpenSaveToolbar() { delete ui; }
