// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class MyMainWindow;

#include "PlotDockBlockBase.h"
#include "block/BlockBase.h"

class XyPlotDockBlock : public PlotDockBlockBase {
  Q_OBJECT

public:
  explicit XyPlotDockBlock( MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString types );

public Q_SLOTS:
  void setAngleCost( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );

  void setRawValues( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );
  void setRawValues2( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );
  void setRawValues3( std::shared_ptr< std::vector< double > >, std::shared_ptr< std::vector< double > > );

private:
  void rescale();
};

class MpcPlotDockBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  MpcPlotDockBlockFactory( QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ) {
    typeColor = TypeColor::Dock;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "MpcPlotDockBlock" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Plots" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Value Plot Dock" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu = nullptr;
};
