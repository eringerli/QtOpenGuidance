// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

class MpcPlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit MpcPlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

public Q_SLOTS:
  void setValues( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );

private:
  void rescale();
};

class MpcPlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  MpcPlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "MpcPlotDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Value Plot Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
