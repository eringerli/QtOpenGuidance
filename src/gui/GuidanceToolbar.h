// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef GUIDANCETOOLBAR_H
#define GUIDANCETOOLBAR_H

#include <QObject>

#include <QGroupBox>

namespace Ui {
  class GuidanceToolbar;
}

class GuidanceToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceToolbar( QWidget* parent = nullptr );
    ~GuidanceToolbar();

    void cbCameraSetChecked( bool enabled );
    void cbSimulatorSetChecked( bool enabled );
    void cbPassesSetChecked( bool enabled );

  private slots:
    void on_cbSimulator_stateChanged( int arg1 );

    void on_btn_settings_clicked();

    void on_cbCamera_stateChanged( int arg1 );

    void on_btn_AB_clicked( bool checked );

    void on_btn_snap_clicked();


    void on_btn_autosteer_clicked( bool checked );

    void on_cbPasses_stateChanged( int arg1 );

  signals:
    void toggleSettings();
    void simulatorChanged( bool );
    void cameraChanged( bool );
    void passesChanged( bool );

    void a_clicked();
    void b_clicked();
    void snap_clicked();

    void autosteerEnabled( bool );

  private:
    Ui::GuidanceToolbar* ui;
};

#endif // GUIDANCETOOLBAR_H
