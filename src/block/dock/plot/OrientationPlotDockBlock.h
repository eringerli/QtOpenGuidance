// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class OrientationPlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit OrientationPlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

public Q_SLOTS:
  void setOrientation( const Eigen::Quaterniond& orientation );

  void setPose( POSE_SIGNATURE_SLOT );

private:
  void rescale();
};

class OrientationPlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  OrientationPlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "OrientationPlotDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Orientation Plot Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
