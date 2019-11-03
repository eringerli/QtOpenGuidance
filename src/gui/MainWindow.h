// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGuiApplication>
#include <QMainWindow>
#include <QSettings>
#include <QStandardPaths>
#include <QEvent>
#include <QWidget>
#include <QObject>
#include <QCloseEvent>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow( QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() )
      : QMainWindow( parent, f ) {}

  protected:
    void closeEvent( QCloseEvent* event ) override {
      emit closed();

      event->accept();
    }

  signals:
    void closed();
};

#endif // MAINWINDOW_H
