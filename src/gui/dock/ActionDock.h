// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QGroupBox>

namespace Ui {
  class ActionDock;
}

class ActionDock : public QGroupBox {
  Q_OBJECT

public:
  explicit ActionDock( QWidget* parent = nullptr );
  ~ActionDock();

  bool          state();
  bool          isCheckable();
  const QString getTheme();

  void setState( const bool );
  void setCheckable( const bool checkable );
  void setTheme( const QString& theme );

private Q_SLOTS:
  void on_pbAction_clicked( const bool checked );

Q_SIGNALS:
  void action( const bool );

private:
  QString         theme;
  Ui::ActionDock* ui;
};
