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
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "SliderDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Input Docks" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Slider Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;

public:
  static KDDockWidgets::DockWidget* firstSliderValueDock;
};
