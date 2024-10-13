// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlotDockBlockBase.h"

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"
#include "qnamespace.h"

KDDockWidgets::QtWidgets::DockWidget* PlotDockBlockBase::firstPlotDock = nullptr;

PlotDockBlockBase::PlotDockBlockBase(
  MyMainWindow* mainWindow, QString uniqueName, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );
  widget = new PlotDock( mainWindow );
}

PlotDockBlockBase::~PlotDockBlockBase() {
  widget->deleteLater();

  if( PlotDockBlockBase::firstPlotDock == dock ) {
    PlotDockBlockBase::firstPlotDock = nullptr;
  }

  dock->deleteLater();
}

void
PlotDockBlockBase::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "XAxisVisible" )]      = getXAxisVisible();
  valuesObject[QStringLiteral( "YAxisVisible" )]      = getYAxisVisible();
  valuesObject[QStringLiteral( "YAxisDescription" )]  = getYAxisDescription();
  valuesObject[QStringLiteral( "AutoscrollEnabled" )] = getAutoscrollEnabled();
  valuesObject[QStringLiteral( "Window" )]            = getWindow();
}

void
PlotDockBlockBase::fromJSON( const QJsonObject& valuesObject ) {
  setXAxisVisible( valuesObject[QStringLiteral( "XAxisVisible" )].toBool( false ) );
  setYAxisVisible( valuesObject[QStringLiteral( "YAxisVisible" )].toBool( true ) );
  setYAxisDescription( valuesObject[QStringLiteral( "YAxisDescription" )].toString( QString() ) );
  setAutoscrollEnabled( valuesObject[QStringLiteral( "AutoscrollEnabled" )].toBool( true ) );
  setWindow( valuesObject[QStringLiteral( "Window" )].toDouble( 20 ) );
}

bool
PlotDockBlockBase::getXAxisVisible() const {
  return widget->getQCustomPlotWidget()->xAxis->visible();
}

bool
PlotDockBlockBase::getYAxisVisible() const {
  return widget->getQCustomPlotWidget()->yAxis->visible();
}

const QString
PlotDockBlockBase::getYAxisDescription() const {
  return widget->getQCustomPlotWidget()->yAxis->label();
}

bool
PlotDockBlockBase::getAutoscrollEnabled() const {
  return autoScrollEnabled;
}

double
PlotDockBlockBase::getWindow() const {
  return window;
}

void
PlotDockBlockBase::setXAxisVisible( const bool visible ) {
  widget->getQCustomPlotWidget()->xAxis->setVisible( visible );
}

void
PlotDockBlockBase::setYAxisVisible( const bool visible ) {
  widget->getQCustomPlotWidget()->yAxis->setVisible( visible );
}

void
PlotDockBlockBase::setYAxisDescription( const QString& description ) {
  widget->getQCustomPlotWidget()->yAxis->setLabel( description );
}

void
PlotDockBlockBase::setAutoscrollEnabled( const bool enabled ) {
  autoScrollEnabled = enabled;

  if( !enabled ) {
    widget->getQCustomPlotWidget()->setInteractions( QCP::Interaction::iRangeDrag | QCP::Interaction::iRangeZoom );
  } else {
    widget->getQCustomPlotWidget()->setInteractions( QCP::Interaction() );
  }

  setNameHelper();
}

void
PlotDockBlockBase::setWindow( const double window ) {
  this->window = window;
}

void
PlotDockBlockBase::clearData() {
  for( int i = 0; i < widget->getQCustomPlotWidget()->graphCount(); ++i ) {
    widget->getQCustomPlotWidget()->graph( i )->setData( QVector< double >(), QVector< double >(), true );
  }
}

void
PlotDockBlockBase::setName( const QString& name ) {
  BlockBase::setName( name );
  setNameHelper();
}

void
PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick( QMouseEvent* event ) {
  if( event->button() == Qt::MouseButton::LeftButton ) {
    setAutoscrollEnabled( !autoScrollEnabled );
  } else {
    clearData();
  }
}

void
PlotDockBlockBase::setNameHelper() {
  dock->setTitle( name() + ( autoScrollEnabled ? QString() : QStringLiteral( " (m" ) ) );
  dock->toggleAction()->setText( QStringLiteral( "Plot: " ) + name() );
}
