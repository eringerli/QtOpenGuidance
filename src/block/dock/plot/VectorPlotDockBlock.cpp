// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "VectorPlotDockBlock.h"

#include "helpers/RateLimiter.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"

#include <QAction>

#include <QMenu>

#include "gui/dock/ActionDock.h"

#include "helpers/anglesHelper.h"

VectorPlotDockBlock::VectorPlotDockBlock( MyMainWindow*              mainWindow,
                                          QString                    uniqueName,
                                          const BlockBaseId          idHint,
                                          const bool                 systemBlock,
                                          const QString              type,
                                          const BlockBase::TypeColor typeColor )
    : PlotDockBlockBase( mainWindow, uniqueName, idHint, systemBlock, type, typeColor ) {
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

std::unique_ptr< BlockBase >
VectorPlotDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< VectorPlotDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

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

  obj->addInputPort( QStringLiteral( "Vector" ), obj.get(), QLatin1StringView( SLOT( addVector( VECTOR_SIGNATURE ) ) ) );

  return obj;
}
