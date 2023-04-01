// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>

#include <QButtonGroup>

namespace Ui {
  class GuidanceTurningToolbar;
}

class GuidanceTurningToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit GuidanceTurningToolbar( QWidget* parent = nullptr );
  ~GuidanceTurningToolbar();

private Q_SLOTS:
  void on_pbLeft_toggled( bool checked );
  void on_pbRight_toggled( bool checked );
  void on_pbPlusRight_clicked();
  void on_pbPlusLeft_clicked();
  void on_pbMinusRight_clicked();
  void on_pbMinusLeft_clicked();

  void on_pbLeft_pressed();
  void on_pbRight_pressed();

public Q_SLOTS:
  void resetTurningState();

Q_SIGNALS:
  void turnLeftToggled( const bool state );
  void turnRightToggled( const bool state );
  void numSkipChanged( const int left, const int right );

private:
  Ui::GuidanceTurningToolbar* ui        = nullptr;
  int                         skipLeft  = 1;
  int                         skipRight = 1;

  QButtonGroup buttonGroupTurn;
};
