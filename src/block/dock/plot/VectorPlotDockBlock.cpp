// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "VectorPlotDockBlock.h"

#include "helpers/RateLimiter.h"
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

VectorPlotDockBlock::VectorPlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow )
    : PlotDockBlockBase( uniqueName, mainWindow ) {
  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();
    graph->setPen( QPen( QColor( 40, 110, 255 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "X" ) );
  }

  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();
    graph->setPen( QPen( QColor( 37, 255, 73 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "Y" ) );
  }

  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();
    graph->setPen( QPen( QColor( 210, 138, 255 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "Z" ) );
  }

  widget->getQCustomPlotWidget()->axisRect()->insetLayout()->setInsetAlignment( 0, Qt::AlignTop | Qt::AlignLeft );
  widget->getQCustomPlotWidget()->legend->setVisible( true );

  QSharedPointer< QCPAxisTickerDateTime > timeTicker( new QCPAxisTickerDateTime );
  timeTicker->setDateTimeFormat( QStringLiteral( "hh:mm:ss" ) );
  widget->getQCustomPlotWidget()->xAxis->setTicker( timeTicker );
}

void
VectorPlotDockBlock::addVector( const Eigen::Vector3d& vector, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::PlotDock ) ) {
    auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 0 )->addData( currentSecsSinceEpoch, vector.x() );
    widget->getQCustomPlotWidget()->graph( 1 )->addData( currentSecsSinceEpoch, vector.y() );
    widget->getQCustomPlotWidget()->graph( 2 )->addData( currentSecsSinceEpoch, vector.z() );

    rescale();
  }
}

void
VectorPlotDockBlock::rescale() {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( currentSecsSinceEpoch - window, currentSecsSinceEpoch );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

QNEBlock*
VectorPlotDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new VectorPlotDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
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

  b->addInputPort( QStringLiteral( "Vector" ), QLatin1String( SLOT( addVector( VECTOR_SIGNATURE ) ) ) );

  b->setBrush( dockColor );

  return b;
}
