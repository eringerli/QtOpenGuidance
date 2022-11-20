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

class MyMainWindow;

#include "block/BlockBase.h"

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

class PlotDock;

class PlotDockBlockBase : public BlockBase {
  Q_OBJECT

public:
  explicit PlotDockBlockBase( const QString& uniqueName, MyMainWindow* mainWindow );

  virtual ~PlotDockBlockBase();

  QJsonObject toJSON() override;
  void        fromJSON( QJsonObject& ) override;

public:
  bool          getXAxisVisible();
  bool          getYAxisVisible();
  const QString getYAxisDescription();
  bool          getAutoscrollEnabled();
  double        getWindow();

  void setXAxisVisible( const bool visible );
  void setYAxisVisible( const bool visible );
  void setYAxisDescription( const QString& description );
  void setAutoscrollEnabled( const bool enabled );
  void setWindow( const double );

public Q_SLOTS:
  void setName( const QString& name ) override;

  void qCustomPlotWidgetMouseDoubleClick( QMouseEvent* );

private:
  void setNameHelper();

public:
  KDDockWidgets::DockWidget* dock   = nullptr;
  PlotDock*                  widget = nullptr;

  QString name;

  bool   autoScrollEnabled = true;
  double window            = 20;
  bool   interactable      = false;

  static KDDockWidgets::DockWidget* firstPlotDock;
};
