// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OpenSaveHelper.h"

#include <QFileDialog>

OpenSaveHelper::OpenSaveHelper( QString title, QString filter, QWidget* mainWindow, QObject* parent )
    : title( title ), filter( filter ), mainWindow( mainWindow ), QObject{ parent } {}

void
OpenSaveHelper::open() {
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow, tr( "Open Field" ), dir, filter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  QString filters = "All Files (*);;" + filter;
  fileDialog->setNameFilter( filters );

  QObject::connect( fileDialog, &QFileDialog::fileSelected, this, &OpenSaveHelper::openFile );

  // connect finished to deleteLater, so the dialog gets deleted when Cancel is pressed
  QObject::connect( fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater );

  fileDialog->open();
}

void
OpenSaveHelper::save() {
  QString dir;
  QString filters  = "All Files (*);;" + filter;
  QString fileName = QFileDialog::getSaveFileName( mainWindow, tr( "Save Field" ), dir, filters, &filter );

  if( !fileName.isEmpty() ) {
    Q_EMIT saveFile( fileName );
  }
}
