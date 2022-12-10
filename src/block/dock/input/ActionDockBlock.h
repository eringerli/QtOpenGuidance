// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Action Dock Block" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Action Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::DockWidget* firstActionDock;
};
