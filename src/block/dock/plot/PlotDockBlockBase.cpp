// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlotDockBlockBase.h"

#include <QBrush>

#include "gui/MyMainWindow.h"
#include "gui/dock/PlotDock.h"

#include "qcustomplot.h"

KDDockWidgets::DockWidget* PlotDockBlockBase::firstPlotDock = nullptr;

PlotDockBlockBase::PlotDockBlockBase( const QString& uniqueName, MyMainWindow* mainWindow ) {
  dock   = new KDDockWidgets::DockWidget( uniqueName );
  widget = new PlotDock( mainWindow );
}

PlotDockBlockBase::~PlotDockBlockBase() {
  widget->deleteLater();

  if( PlotDockBlockBase::firstPlotDock == dock ) {
    PlotDockBlockBase::firstPlotDock = nullptr;
  }

  dock->deleteLater();
}

QJsonObject
PlotDockBlockBase::toJSON() {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "XAxisVisible" )]      = getXAxisVisible();
  valuesObject[QStringLiteral( "YAxisVisible" )]      = getYAxisVisible();
  valuesObject[QStringLiteral( "YAxisDescription" )]  = getYAxisDescription();
  valuesObject[QStringLiteral( "AutoscrollEnabled" )] = getAutoscrollEnabled();
  valuesObject[QStringLiteral( "Window" )]            = getWindow();

  return valuesObject;
}

void
PlotDockBlockBase::fromJSON( QJsonObject& valuesObject ) {
  setXAxisVisible( valuesObject[QStringLiteral( "XAxisVisible" )].toBool( false ) );
  setYAxisVisible( valuesObject[QStringLiteral( "YAxisVisible" )].toBool( true ) );
  setYAxisDescription( valuesObject[QStringLiteral( "YAxisDescription" )].toString( QString() ) );
  setAutoscrollEnabled( valuesObject[QStringLiteral( "AutoscrollEnabled" )].toBool( true ) );
  setWindow( valuesObject[QStringLiteral( "Window" )].toDouble( 20 ) );
}

bool
PlotDockBlockBase::getXAxisVisible() {
  return widget->getQCustomPlotWidget()->xAxis->visible();
}

bool
PlotDockBlockBase::getYAxisVisible() {
  return widget->getQCustomPlotWidget()->yAxis->visible();
}

const QString
PlotDockBlockBase::getYAxisDescription() {
  return widget->getQCustomPlotWidget()->yAxis->label();
}

bool
PlotDockBlockBase::getAutoscrollEnabled() {
  return autoScrollEnabled;
}

double
PlotDockBlockBase::getWindow() {
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
PlotDockBlockBase::setName( const QString& name ) {
  this->name = name;
  setNameHelper();
}

void
PlotDockBlockBase::qCustomPlotWidgetMouseDoubleClick( QMouseEvent* ) {
  setAutoscrollEnabled( !autoScrollEnabled );
}

void
PlotDockBlockBase::setNameHelper() {
  dock->setTitle( name + ( autoScrollEnabled ? QString() : QStringLiteral( " (m)" ) ) );
  dock->toggleAction()->setText( QStringLiteral( "Plot: " ) + name );
}
