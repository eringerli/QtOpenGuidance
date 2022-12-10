// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "block/BlockBase.h"

#include <kddockwidgets/KDDockWidgets.h>

class MyMainWindow;
class QMenu;
namespace KDDockWidgets {
  class DockWidget;
}
class SliderDock;

class SliderDockBlock : public BlockBase {
  Q_OBJECT

public:
  explicit SliderDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

  ~SliderDockBlock();

  virtual void emitConfigSignals() override;

  virtual QJsonObject toJSON() override;
  virtual void        fromJSON( QJsonObject& ) override;

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setValue( const double );

  void valueChangedProxy( double );

Q_SIGNALS:
  void valueChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  SliderDock* widget = nullptr;

  KDDockWidgets::DockWidget* dock = nullptr;
};

class SliderDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  SliderDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SliderDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Slider Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::DockWidget* firstSliderValueDock;
};
