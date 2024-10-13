// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XyPlotDockBlock.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"

#include <QAction>

#include <QMenu>

#include "gui/dock/ActionDock.h"

#include "helpers/anglesHelper.h"

XyPlotDockBlock::XyPlotDockBlock(
  MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString types )
    : PlotDockBlockBase( mainWindow, uniqueName, idHint, systemBlock, types ) {
  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph( 0 )->setPen( QPen( QColor( 40, 110, 255 ) ) );
  widget->getQCustomPlotWidget()->graph( 0 )->setLineStyle( QCPGraph::LineStyle::lsLine /*lsImpulse*/ );

  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph( 1 )->setPen( QPen( QColor( 110, 40, 0 ) ) );
  widget->getQCustomPlotWidget()->graph( 1 )->setLineStyle( QCPGraph::LineStyle::lsLine /*lsImpulse*/ );

  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph( 2 )->setPen( QPen( QColor( 200, 200, 0 ) ) );
  widget->getQCustomPlotWidget()->graph( 2 )->setLineStyle( QCPGraph::LineStyle::lsLine /*lsImpulse*/ );

  QSharedPointer< QCPAxisTicker > ticker( new QCPAxisTicker );
  ticker->setTickCount( 10 );
  widget->getQCustomPlotWidget()->xAxis->setTicker( ticker );
}

void
XyPlotDockBlock::setAngleCost( std::shared_ptr< std::vector< double > > angles, std::shared_ptr< std::vector< double > > costs ) {
  //  std::map< double, double > anglesCosts;

  QVector< double > keys;
  QVector< double > values;
  for( int i = 0; i < angles->size(); ++i ) {
    keys.push_back( radiansToDegrees( angles->at( i ) ) );
    values.push_back( costs->at( i ) );
    //    anglesCosts.insert_or_assign( radiansToDegrees( angles->at( i ) ), costs->at( i ) );
  }

  widget->getQCustomPlotWidget()->graph( 0 )->setData( keys, values, false );

  //  for( auto element : anglesCosts ) {
  //    widget->getQCustomPlotWidget()->graph( 0 )->addData( element.first, element.second );
  //  }

  rescale();
}

void
XyPlotDockBlock::setRawValues( std::shared_ptr< std::vector< double > > x, std::shared_ptr< std::vector< double > > y ) {
  QVector< double > keys;
  QVector< double > values;
  for( int i = 0; i < x->size(); ++i ) {
    keys.push_back( x->at( i ) );
    values.push_back( y->at( i ) );
  }

  widget->getQCustomPlotWidget()->graph( 0 )->setData( keys, values, false );

  rescale();
}

void
XyPlotDockBlock::setRawValues2( std::shared_ptr< std::vector< double > > x, std::shared_ptr< std::vector< double > > y ) {
  QVector< double > keys;
  QVector< double > values;
  for( int i = 0; i < x->size(); ++i ) {
    keys.push_back( x->at( i ) );
    values.push_back( y->at( i ) );
  }

  widget->getQCustomPlotWidget()->graph( 1 )->setData( keys, values, false );

  rescale();
}

void
XyPlotDockBlock::setRawValues3( std::shared_ptr< std::vector< double > > x, std::shared_ptr< std::vector< double > > y ) {
  QVector< double > keys;
  QVector< double > values;
  for( int i = 0; i < x->size(); ++i ) {
    keys.push_back( x->at( i ) );
    values.push_back( y->at( i ) );
  }

  widget->getQCustomPlotWidget()->graph( 2 )->setData( keys, values, false );

  rescale();
}

void
XyPlotDockBlock::rescale() {
  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( -40, 40 );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

std::unique_ptr< BlockBase >
MpcPlotDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< XyPlotDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  if( PlotDockBlockBase::firstPlotDock == nullptr ) {
    mainWindow->addDockWidget( obj->dock, location );
    PlotDockBlockBase::firstPlotDock = obj->dock;
  } else {
    mainWindow->addDockWidget( obj->dock, KDDockWidgets::Location_OnBottom, PlotDockBlockBase::firstPlotDock );
  }

  QObject::connect(
    obj->widget->getQCustomPlotWidget(), &QCustomPlot::mouseDoubleClick, obj.get(), &PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick );

  return obj;
}
