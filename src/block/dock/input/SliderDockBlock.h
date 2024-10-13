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
  explicit SliderDockBlock(
    MyMainWindow* mainWindow, const QString& uniqueName, const int idHint, const bool systemBlock, const QString type );

  ~SliderDockBlock();

  virtual void emitConfigSignals() override;

  virtual void toJSON( QJsonObject& json ) const override;
  virtual void fromJSON( const QJsonObject& ) override;

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setValue( const double );

  void valueChangedProxy( double );

Q_SIGNALS:
  void valueChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  SliderDock* widget = nullptr;

  KDDockWidgets::QtWidgets::DockWidget* dock = nullptr;
};

class SliderDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  SliderDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "SliderDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Slider Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::QtWidgets::DockWidget* firstSliderValueDock;
};
