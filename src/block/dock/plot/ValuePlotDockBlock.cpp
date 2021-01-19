// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "ValuePlotDockBlock.h"

#include "qneblock.h"
#include "qneport.h"

#include "gui/dock/PlotDock.h"
#include "gui/MyMainWindow.h"

#include "qcustomplot.h"

#include <QAction>
#include <QMenu>
#include <QBrush>

#include "gui/dock/ActionDock.h"

#include "helpers/anglesHelper.h"

ValuePlotDockBlock::ValuePlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow )
  : PlotDockBlockBase( uniqueName, mainWindow ) {

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

  QSharedPointer<QCPAxisTickerDateTime> timeTicker( new QCPAxisTickerDateTime );
  timeTicker->setDateTimeFormat( QStringLiteral( "hh:mm:ss" ) );
  widget->getQCustomPlotWidget()->xAxis->setTicker( timeTicker );
}

void ValuePlotDockBlock::addValue0( const double value ) {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  widget->getQCustomPlotWidget()->graph( 0 )->addData( currentSecsSinceEpoch, value );

  rescale();
}

void ValuePlotDockBlock::addValue1( const double value ) {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  widget->getQCustomPlotWidget()->graph( 1 )->addData( currentSecsSinceEpoch, value );

  rescale();
}

void ValuePlotDockBlock::addValue2( const double value ) {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  widget->getQCustomPlotWidget()->graph( 2 )->addData( currentSecsSinceEpoch, value );

  rescale();
}

void ValuePlotDockBlock::addValue3( const double value ) {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  widget->getQCustomPlotWidget()->graph( 3 )->addData( currentSecsSinceEpoch, value );

  rescale();
}

void ValuePlotDockBlock::rescale() {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( currentSecsSinceEpoch - window, currentSecsSinceEpoch );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

QNEBlock* ValuePlotDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new ValuePlotDockBlock( getNameOfFactory() + QString::number( id ),
                                         mainWindow );
  auto* b = createBaseBlock( scene, object, id );

  object->dock->setTitle( getNameOfFactory() );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  if( PlotDockBlockBase::firstPlotDock == nullptr ) {
    mainWindow->addDockWidget( object->dock, location );
    PlotDockBlockBase::firstPlotDock = object->dock;
  } else {
    mainWindow->addDockWidget( object->dock, KDDockWidgets::Location_OnBottom, PlotDockBlockBase::firstPlotDock );
  }

  QObject::connect( object->widget->getQCustomPlotWidget(), &QCustomPlot::mouseDoubleClick, object, &PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick );

  b->addInputPort( QStringLiteral( "Number 0" ), QLatin1String( SLOT( addValue0( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Number 1" ), QLatin1String( SLOT( addValue1( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Number 2" ), QLatin1String( SLOT( addValue2( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Number 3" ), QLatin1String( SLOT( addValue3( const double ) ) ) );

  b->setBrush( dockColor );

  return b;
}
