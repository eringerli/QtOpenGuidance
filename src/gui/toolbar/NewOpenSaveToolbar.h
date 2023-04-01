// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QGroupBox>
class QMenu;

namespace Ui {
  class NewOpenSaveToolbar;
}

class NewOpenSaveToolbar : public QGroupBox {
  Q_OBJECT

public:
  explicit NewOpenSaveToolbar( QWidget* parent = nullptr );
  ~NewOpenSaveToolbar();

public:
  QMenu* openMenu = nullptr;
  QMenu* newMenu  = nullptr;
  QMenu* saveMenu = nullptr;

private:
  Ui::NewOpenSaveToolbar* ui;
};
