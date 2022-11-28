// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>

namespace Ui {
  class ValueDock;
}

class ValueDock : public QGroupBox {
  Q_OBJECT

public:
  explicit ValueDock( QWidget* parent = nullptr );
  ~ValueDock();

public Q_SLOTS:
  void setMeter( const double );
  void setFontOfLabel( const QFont& font );

public:
  const QFont& fontOfLabel();

  int     precision  = 0;
  int     fieldWidth = 0;
  double  scale      = 1;
  QString unit;
  bool    unitEnabled = false;

private:
  Ui::ValueDock* ui = nullptr;
};
