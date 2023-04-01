// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
