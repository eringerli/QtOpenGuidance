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

#include <QGroupBox>
#include <QHBoxLayout>

#include <QButtonGroup>

namespace Ui {
  class GuidanceTurning;
}

class GuidanceTurning : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceTurning( QWidget* parent = nullptr );
    ~GuidanceTurning();

  private Q_SLOTS:
    void on_pbLeft_toggled( bool checked );
    void on_pbRight_toggled( bool checked );
    void on_pbPlusRight_clicked();
    void on_pbPlusLeft_clicked();
    void on_pbMinusRight_clicked();
    void on_pbMinusLeft_clicked();

    void on_pbLeft_pressed();
    void on_pbRight_pressed();

  public Q_SLOTS:
    void resetTurningState();

  Q_SIGNALS:
    void turnLeftToggled( const bool state );
    void turnRightToggled( const bool state );
    void numSkipChanged( const int left, const int right );

  private:
    Ui::GuidanceTurning* ui = nullptr;
    int skipLeft = 1;
    int skipRight = 1;

    QButtonGroup buttonGroupTurn;
};
