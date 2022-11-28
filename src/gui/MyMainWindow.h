// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class QWindow;
class QCloseEvent;

#include <kddockwidgets/MainWindow.h>

class MyMainWindow : public KDDockWidgets::MainWindow {
  Q_OBJECT

public:
  explicit MyMainWindow( const QString&                   uniqueName,
                         KDDockWidgets::MainWindowOptions options,
                         const QString&                   affinityName = {},
                         QWidget*                         parent       = nullptr );

  void readSettings();

protected:
  void closeEvent( QCloseEvent* event ) override;

Q_SIGNALS:
  void closed();
};
