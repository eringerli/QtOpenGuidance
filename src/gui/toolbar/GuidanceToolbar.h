// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QGroupBox>

#include "helpers/signatures.h"
#include "kinematic/CalculationOptions.h"

class QMenu;

namespace Ui {
  class GuidanceToolbar;
}

class GuidanceToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit GuidanceToolbar( QWidget* parent = nullptr );
  ~GuidanceToolbar();

  void cbSimulatorSetChecked( const bool enabled );

private Q_SLOTS:
  void on_cbSimulator_stateChanged( int arg1 );

  void on_pbSettings_clicked();

Q_SIGNALS:
  void toggleSettings();
  void simulatorChanged( ACTION_SIGNATURE_SIGNAL );

public:
  QMenu* menu = nullptr;

private:
  Ui::GuidanceToolbar* ui = nullptr;
};
