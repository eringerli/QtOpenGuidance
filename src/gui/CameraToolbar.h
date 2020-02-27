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
  class CameraToolbar;
}

class CameraToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit CameraToolbar( QWidget* parent = nullptr );
    ~CameraToolbar();

  private slots:
    void on_pbUp_clicked();

    void on_pbDown_clicked();

    void on_pbZoomIn_clicked();

    void on_pbZoomOut_clicked();

    void on_pbPanLeft_clicked();

    void on_pbPanRight_clicked();

    void on_pbReset_clicked();

    void on_comboBox_currentIndexChanged( int index );

  signals:
    void zoomIn();
    void zoomOut();
    void tiltUp();
    void tiltDown();
    void panRight();
    void panLeft();
    void resetCamera();
    void setMode( int );

  private:
    Ui::CameraToolbar* ui = nullptr;
};

