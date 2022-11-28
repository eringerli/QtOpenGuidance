// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
