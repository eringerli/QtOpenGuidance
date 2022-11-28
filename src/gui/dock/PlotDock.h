// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>

class QCustomPlot;

namespace Ui {
  class PlotDock;
}

class PlotDock : public QGroupBox {
  Q_OBJECT

public:
  explicit PlotDock( QWidget* parent = nullptr );
  ~PlotDock();

public:
  QCustomPlot* getQCustomPlotWidget();

private:
  Ui::PlotDock* ui;
};
