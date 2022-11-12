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

#pragma once

#include <QObject>

#include "block/BlockBase.h"

class MyMainWindow;
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

class ActionDock;

class ActionDockBlock : public BlockBase {
  Q_OBJECT

public:
  explicit ActionDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );
  ~ActionDockBlock();

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setCheckable( const bool checkable );

  void setTheme( const QString& theme );

Q_SIGNALS:
  void action( ACTION_SIGNATURE_SIGNAL );

public:
  ActionDock*                widget = nullptr;
  KDDockWidgets::DockWidget* dock   = nullptr;
};

class ActionDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  ActionDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Action Dock Block" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Action Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::DockWidget* firstActionDock;
};
