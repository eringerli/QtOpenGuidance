// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MpcPlotDockBlock.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"

#include <QAction>
#include <QBrush>
#include <QMenu>

#include "gui/dock/ActionDock.h"

#include "helpers/anglesHelper.h"

MpcPlotDockBlock::MpcPlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow ) : PlotDockBlockBase( uniqueName, mainWindow ) {
  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph()->setPen( QPen( QColor( 40, 110, 255 ) ) );
  widget->getQCustomPlotWidget()->graph()->setLineStyle( QCPGraph::LineStyle::/*lsLine*/ lsImpulse );

  QSharedPointer< QCPAxisTicker > ticker( new QCPAxisTicker );
  ticker->setTickCount( 10 );
  widget->getQCustomPlotWidget()->xAxis->setTicker( ticker );
}

void
MpcPlotDockBlock::setValues( std::shared_ptr< std::vector< double > > angles, std::shared_ptr< std::vector< double > > costs ) {
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
MpcPlotDockBlock::rescale() {
  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( -40, 40 );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

QNEBlock*
MpcPlotDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new MpcPlotDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

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
    obj->widget->getQCustomPlotWidget(), &QCustomPlot::mouseDoubleClick, obj, &PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick );

  b->setBrush( dockColor );

  return b;
}
