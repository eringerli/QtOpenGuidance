// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>
class QBoxLayout;

namespace Ui {
  class FieldsToolbar;
}

class FieldsToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit FieldsToolbar( QWidget* parent = nullptr );
  ~FieldsToolbar();

Q_SIGNALS:
  void continousRecordToggled( const bool );
  void recordPoint();

  void recordOnEdgeOfImplementChanged( const bool right );

private slots:
  void on_pbEdgeOfImplement_clicked( bool checked );

private:
  Ui::FieldsToolbar* ui        = nullptr;
  QBoxLayout*        boxLayout = nullptr;
};
