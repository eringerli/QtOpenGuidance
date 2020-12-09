// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#pragma once

#include <QObject>

#include <QGroupBox>
#include <QMenu>

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
    void simulatorChanged( const bool );

  public:
    QMenu* menu = nullptr;

  private:
    Ui::GuidanceToolbar* ui = nullptr;
};
