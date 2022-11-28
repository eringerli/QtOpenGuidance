// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

class ValuePlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit ValuePlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow );

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
      : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ) {}

  QString getNameOfFactory() override { return QStringLiteral( "ValuePlotDockBlock" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "Value Plot Dock" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
