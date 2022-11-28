// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>

namespace Ui {
  class ThreeValuesDock;
}

class ThreeValuesDock : public QGroupBox {
  Q_OBJECT

public:
  explicit ThreeValuesDock( QWidget* parent = nullptr );
  ~ThreeValuesDock();

public Q_SLOTS:
  void setFontOfLabel( const QFont& font );

  void setValues( const double, const double, const double );
  void setDescriptions( const QString& first, const QString& second, const QString& third );

public:
  const QFont& fontOfLabel();

  int     precision  = 0;
  int     fieldWidth = 0;
  double  scale      = 1;
  QString unit;
  bool    unitEnabled = false;

private:
  Ui::ThreeValuesDock* ui = nullptr;
};
