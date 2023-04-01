// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QGroupBox>

namespace Ui {
  class GlobalPlannerToolbar;
}

class GlobalPlannerToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit GlobalPlannerToolbar( QWidget* parent = nullptr );
  ~GlobalPlannerToolbar();
  void resetToolbar();
  void toolbarToAdditionalPointSet();

private Q_SLOTS:
  void on_pbAB_clicked( bool checked );
  void on_pbSnap_clicked();

  void on_pbAbContinuous_clicked( bool checked );

Q_SIGNALS:
  void setAPoint();
  void setBPoint();
  void setAdditionalPoint();
  void setAdditionalPointsContinous( const bool );
  void snap();

private:
  Ui::GlobalPlannerToolbar* ui;
};
