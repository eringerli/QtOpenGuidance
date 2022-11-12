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

#include "MyMainWindow.h"

#include <QObject>
#include <QSettings>
#include <QStandardPaths>

#include <QCloseEvent>
#include <QEvent>

MyMainWindow::MyMainWindow( const QString&                   uniqueName,
                            KDDockWidgets::MainWindowOptions options,
                            const QString&                   affinityName,
                            QWidget*                         parent )
    : MainWindow( uniqueName, options, parent ) {
  setAffinities( { affinityName } );
}

void
MyMainWindow::readSettings() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  restoreGeometry( settings.value( "geometry" ).toByteArray() );
  restoreState( settings.value( "windowState" ).toByteArray() );
}

void
MyMainWindow::closeEvent( QCloseEvent* event ) {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini", QSettings::IniFormat );

  settings.setValue( "geometry", saveGeometry() );
  settings.setValue( "windowState", saveState() );
  settings.sync();

  QMainWindow::closeEvent( event );
}
