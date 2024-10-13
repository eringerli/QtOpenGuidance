// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"

class ValuePlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit ValuePlotDockBlock( MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type );

  RateLimiter rateLimiterValue0;
  RateLimiter rateLimiterValue1;
  RateLimiter rateLimiterValue2;
  RateLimiter rateLimiterValue3;

public Q_SLOTS:
  void addValue0( NUMBER_SIGNATURE_SLOT );
  void addValue1( NUMBER_SIGNATURE_SLOT );
  void addValue2( NUMBER_SIGNATURE_SLOT );
  void addValue3( NUMBER_SIGNATURE_SLOT );

private:
  void rescale();
};

class ValuePlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  ValuePlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "ValuePlotDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Value Plot Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
