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
  explicit ActionDockBlock(
    MyMainWindow* mainWindow, const QString& uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type );
  ~ActionDockBlock();

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setCheckable( const bool checkable );

  void setTheme( const QString& theme );

Q_SIGNALS:
  void action( ACTION_SIGNATURE_SIGNAL );

public:
  ActionDock*                           widget = nullptr;
  KDDockWidgets::QtWidgets::DockWidget* dock   = nullptr;
};

class ActionDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  ActionDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Action Dock Block" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Action Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::QtWidgets::DockWidget* firstActionDock;
};
