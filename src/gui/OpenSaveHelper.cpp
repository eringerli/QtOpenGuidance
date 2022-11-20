// Copyright( C ) 2022 Christian Riggenbach
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
