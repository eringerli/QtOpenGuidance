// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"

class OrientationPlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit OrientationPlotDockBlock( MyMainWindow*              mainWindow,
                                     QString                    uniqueName,
                                     const BlockBaseId          idHint,
                                     const bool                 systemBlock,
                                     const QString              type,
                                     const BlockBase::TypeColor typeColor );

public Q_SLOTS:
  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );

private:
  void rescale();

  RateLimiter rateLimiter;
};

class OrientationPlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  OrientationPlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = BlockBase::TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "OrientationPlotDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Orientation Plot Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
