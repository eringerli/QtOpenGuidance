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
  explicit PlotDockBlockBase( MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type );

  virtual ~PlotDockBlockBase();

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

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
  KDDockWidgets::QtWidgets::DockWidget* dock   = nullptr;
  PlotDock*                             widget = nullptr;

  bool   autoScrollEnabled = true;
  double window            = 20;
  bool   interactable      = false;

  static KDDockWidgets::QtWidgets::DockWidget* firstPlotDock;
};
