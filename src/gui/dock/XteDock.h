// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>

namespace Ui {
  class XteDock;
}

class XteDock : public QGroupBox {
  Q_OBJECT

public:
  explicit XteDock( QWidget* parent = nullptr );
  ~XteDock();

public Q_SLOTS:
  void setXte( const double );
  void setName( const QString& name );
  void setFontOfLabel( const QFont& font );

public:
  const QFont& fontOfLabel();

  int     precision   = 0;
  int     fieldWidth  = 0;
  double  scale       = 100;
  QString unit        = QStringLiteral( "cm" );
  bool    unitEnabled = false;

private:
  Ui::XteDock* ui = nullptr;
};
