// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ValuePlotDockBlock.h"

#include "helpers/RateLimiter.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"

#include <QAction>

#include <QMenu>

#include "gui/dock/ActionDock.h"

#include "helpers/anglesHelper.h"

ValuePlotDockBlock::ValuePlotDockBlock(
  MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : PlotDockBlockBase( mainWindow, uniqueName, idHint, systemBlock, type ) {
  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph()->setPen( QPen( QColor( 40, 110, 255 ) ) );
  widget->getQCustomPlotWidget()->graph()->setLineStyle( QCPGraph::LineStyle::lsLine );

  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph()->setPen( QPen( QColor( 37, 255, 73 ) ) );
  widget->getQCustomPlotWidget()->graph()->setLineStyle( QCPGraph::LineStyle::lsLine );

  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph()->setPen( QPen( QColor( 210, 138, 255 ) ) );
  widget->getQCustomPlotWidget()->graph()->setLineStyle( QCPGraph::LineStyle::lsLine );

  widget->getQCustomPlotWidget()->addGraph();
  widget->getQCustomPlotWidget()->graph()->setPen( QPen( QColor( 255, 90, 126 ) ) );
  widget->getQCustomPlotWidget()->graph()->setLineStyle( QCPGraph::LineStyle::lsLine );

  QSharedPointer< QCPAxisTickerDateTime > timeTicker( new QCPAxisTickerDateTime );
  timeTicker->setDateTimeFormat( QStringLiteral( "hh:mm:ss" ) );
  widget->getQCustomPlotWidget()->xAxis->setTicker( timeTicker );
}

void
ValuePlotDockBlock::addValue0( const double value, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiterValue0.expired( RateLimiter::Type::PlotDock ) ) {
    auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 0 )->addData( currentSecsSinceEpoch, value );

    rescale();
  }
}

void
ValuePlotDockBlock::addValue1( const double value, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiterValue1.expired( RateLimiter::Type::PlotDock ) ) {
    auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 1 )->addData( currentSecsSinceEpoch, value );

    rescale();
  }
}

void
ValuePlotDockBlock::addValue2( const double value, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiterValue2.expired( RateLimiter::Type::PlotDock ) ) {
    auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 2 )->addData( currentSecsSinceEpoch, value );

    rescale();
  }
}

void
ValuePlotDockBlock::addValue3( const double value, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiterValue3.expired( RateLimiter::Type::PlotDock ) ) {
    auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 3 )->addData( currentSecsSinceEpoch, value );

    rescale();
  }
}

void
ValuePlotDockBlock::rescale() {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( currentSecsSinceEpoch - window, currentSecsSinceEpoch );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

std::unique_ptr< BlockBase >
ValuePlotDockBlockFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< ValuePlotDockBlock >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

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

  obj->addInputPort( QStringLiteral( "Number 0" ), obj.get(), QLatin1StringView( SLOT( addValue0( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Number 1" ), obj.get(), QLatin1StringView( SLOT( addValue1( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Number 2" ), obj.get(), QLatin1StringView( SLOT( addValue2( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Number 3" ), obj.get(), QLatin1StringView( SLOT( addValue3( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
