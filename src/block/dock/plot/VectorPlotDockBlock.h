// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"

class VectorPlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit VectorPlotDockBlock(
    MyMainWindow* mainWindow, QString uniqueName, const int idHint, const bool systemBlock, const QString type );

  RateLimiter rateLimiter;

public Q_SLOTS:
  void addVector( VECTOR_SIGNATURE_SLOT );

private:
  void rescale();
};

class VectorPlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  VectorPlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "VectorPlotDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Vector Plot Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
