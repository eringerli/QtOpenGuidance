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

#include "OrientationPlotDockBlock.h"

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

  QSharedPointer<QCPAxisTickerDateTime> timeTicker( new QCPAxisTickerDateTime );
  timeTicker->setDateTimeFormat( QStringLiteral( "hh:mm:ss" ) );
  widget->getQCustomPlotWidget()->xAxis->setTicker( timeTicker );
}

void OrientationPlotDockBlock::setOrientation( const Eigen::Quaterniond& orientation ) {
  const auto taitBryanDegrees = radiansToDegrees( quaternionToTaitBryan( orientation ) );
  double currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  widget->getQCustomPlotWidget()->graph( 0 )->addData( currentSecsSinceEpoch, getRoll( taitBryanDegrees ) );
  widget->getQCustomPlotWidget()->graph( 1 )->addData( currentSecsSinceEpoch, getPitch( taitBryanDegrees ) );
  widget->getQCustomPlotWidget()->graph( 2 )->addData( currentSecsSinceEpoch, getYaw( taitBryanDegrees ) );

  rescale();
}

void OrientationPlotDockBlock::setPose( const Eigen::Vector3d&, const Eigen::Quaterniond& orientation, const PoseOption::Options& ) {
  setOrientation( orientation );
}

void OrientationPlotDockBlock::rescale() {
  auto currentSecsSinceEpoch = double( QDateTime::currentMSecsSinceEpoch() ) / 1000;

  if( autoScrollEnabled ) {
    widget->getQCustomPlotWidget()->xAxis->setRange( currentSecsSinceEpoch - window, currentSecsSinceEpoch );
    widget->getQCustomPlotWidget()->yAxis->rescale( true );
  }

  widget->getQCustomPlotWidget()->replot();
}

QNEBlock* OrientationPlotDockBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  if( id != 0 && !isIdUnique( scene, id ) ) {
    id = QNEBlock::getNextUserId();
  }

  auto* object = new OrientationPlotDockBlock( getNameOfFactory() + QString::number( id ),
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

  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  b->setBrush( dockColor );

  return b;
}
