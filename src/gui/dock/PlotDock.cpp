// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PlotDock.h"
#include "ui_PlotDock.h"

PlotDock::PlotDock( QWidget* parent ) : QGroupBox( parent ), ui( new Ui::PlotDock ) { ui->setupUi( this ); }

PlotDock::~PlotDock() { delete ui; }

QCustomPlot*
PlotDock::getQCustomPlotWidget() {
  return ui->customPlot;
}
