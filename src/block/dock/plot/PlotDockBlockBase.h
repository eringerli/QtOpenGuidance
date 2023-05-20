// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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

  QJsonObject toJSON() const override;
  void        fromJSON( QJsonObject& ) override;

public:
  bool          getXAxisVisible() const;
  bool          getYAxisVisible() const;
  const QString getYAxisDescription() const;
  bool          getAutoscrollEnabled() const;
  double        getWindow() const;

  void setXAxisVisible( const bool visible );
  void setYAxisVisible( const bool visible );
  void setYAxisDescription( const QString& description );
  void setAutoscrollEnabled( const bool enabled );
  void setWindow( const double );

  virtual void clearData();

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
