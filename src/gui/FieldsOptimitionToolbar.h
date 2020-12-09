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
  class FieldsOptimitionToolbar;
}

class FieldsOptimitionToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit FieldsOptimitionToolbar( QWidget* parent = nullptr );
    ~FieldsOptimitionToolbar();

    enum class AlphaType : uint_fast8_t {
      None = 0,
      Optimal,
      Solid,
      Custom
    };

  Q_SIGNALS:
    void recalculateField();
    void recalculateFieldSettingsChanged( FieldsOptimitionToolbar::AlphaType alphaType, double customAlpha, double maxDeviation, double distanceBetweenConnectPoints );

  public Q_SLOTS:
    void setAlpha( const double optimal, const double solid );

  private Q_SLOTS:
    void on_pbRecalculate_clicked();

    void on_cbAlphaShape_currentTextChanged( const QString& );

    void on_cbConnectEndToStart_stateChanged( int );

    void on_dsbDistanceConnectingPoints_valueChanged( double );

    void on_dsbAlpha_valueChanged( double arg1 );

    void on_dsbMaxDeviation_valueChanged( double arg1 );

  private:
    Ui::FieldsOptimitionToolbar* ui = nullptr;

    double optimalAlpha = 20;
    double solidAlpha = 100;
};
