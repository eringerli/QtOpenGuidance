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

namespace Ui {
  class SimulatorToolbar;
}

class SimulatorToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit SimulatorToolbar( QWidget* parent = nullptr );
    ~SimulatorToolbar();

  private slots:
    void on_sl_velocity_valueChanged( int value );
    void on_sl_steerangle_valueChanged( int value );
    void on_sl_frequency_valueChanged( int value );

    void on_dsb_velocity_valueChanged( double val );
    void on_dsb_steerangle_valueChanged( double val );
    void on_dsb_frequency_valueChanged( double val );

    void on_pb_velocityReset_clicked();
    void on_pb_steerangleReset_clicked();
    void on_pb_frequencyReset_clicked();

  signals:
    void velocityChanged( double );
    void steerangleChanged( double );
    void frequencyChanged( double );

  private:
    Ui::SimulatorToolbar* ui = nullptr;
};
