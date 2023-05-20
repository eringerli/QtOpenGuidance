// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OrientationPlotDockBlock.h"

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

OrientationPlotDockBlock::OrientationPlotDockBlock( const QString& uniqueName, MyMainWindow* mainWindow )
    : PlotDockBlockBase( uniqueName, mainWindow ) {
  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();

    graph->setPen( QPen( QColor( 40, 110, 255 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "Roll" ) );
  }

  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();

    graph->setPen( QPen( QColor( 37, 255, 73 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "Pitch" ) );
  }

  {
    auto graph = widget->getQCustomPlotWidget()->addGraph();

    graph->setPen( QPen( QColor( 210, 138, 255 ) ) );
    graph->setLineStyle( QCPGraph::LineStyle::lsLine );
    graph->setName( QStringLiteral( "Yaw" ) );
  }

  widget->getQCustomPlotWidget()->axisRect()->insetLayout()->setInsetAlignment( 0, Qt::AlignTop | Qt::AlignLeft );
  widget->getQCustomPlotWidget()->legend->setVisible( true );

  QSharedPointer< QCPAxisTickerDateTime > timeTicker( new QCPAxisTickerDateTime );
  timeTicker->setDateTimeFormat( QStringLiteral( "hh:mm:ss" ) );
  widget->getQCustomPlotWidget()->xAxis->setTicker( timeTicker );
}

void
OrientationPlotDockBlock::setOrientation( const Eigen::Quaterniond& orientation, const CalculationOption::Options ) {
  if( rateLimiter.expired( RateLimiter::Type::PlotDock ) ) {
    const auto taitBryanDegrees      = radiansToDegrees( quaternionToTaitBryan( orientation ) );
    double     currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

    widget->getQCustomPlotWidget()->graph( 0 )->addData( currentSecsSinceEpoch, getRoll( taitBryanDegrees ) );
    widget->getQCustomPlotWidget()->graph( 1 )->addData( currentSecsSinceEpoch, getPitch( taitBryanDegrees ) );
    widget->getQCustomPlotWidget()->graph( 2 )->addData( currentSecsSinceEpoch, getYaw( taitBryanDegrees ) );

    rescale();
  }
}

void
OrientationPlotDockBlock::setPose( const Eigen::Vector3d&,
                                   const Eigen::Quaterniond&        orientation,
                                   const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::PlotDock ) ) {
    setOrientation( orientation );
  }
}

void
OrientationPlotDockBlock::rescale() {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( currentSecsSinceEpoch - window, currentSecsSinceEpoch );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

QNEBlock*
OrientationPlotDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* obj = new OrientationPlotDockBlock( getNameOfFactory() + QString::number( id ), mainWindow );
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

  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->setBrush( dockColor );

  return b;
}
