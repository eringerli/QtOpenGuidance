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

namespace Ui {
  class PassToolbar;
}

class PassToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit PassToolbar( QWidget* parent = nullptr );
    ~PassToolbar();

    void emitPassSettings();

  Q_SIGNALS:
    void passSettingsChanged( const int, const int, const bool, const bool );
    void minimalSkipChanged( const int );
    void passNumberChanged( const int );

  private Q_SLOTS:
    void on_sbForwardPasses_valueChanged( int arg1 );

    void on_sbReversePasses_valueChanged( int arg1 );

    void on_rbStartRight_toggled( bool checked );

    void on_pbResetForwardPasses_clicked();

    void on_pbResetReversePasses_clicked();

    void on_pbSetReverseToForwardPasses_clicked();

    void on_pbResetPassNumber_clicked();

    void on_pbSetPassNumber_clicked();

    void on_rbStartLeft_toggled( bool checked );

    void on_cbMirror_stateChanged( int arg1 );

  private:
    Ui::PassToolbar* ui = nullptr;
};
