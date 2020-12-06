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
  class SliderDock;
}

class SliderDock : public QGroupBox {
    Q_OBJECT

  public:
    explicit SliderDock( QWidget* parent = nullptr );
    ~SliderDock();

    void setValue( double value );
    void setDecimals( const int decimals );
    void setMaximum( const double maximum );
    void setMinimum( const double minimum );
    void setDefaultValue( const double defaultValue );
    void setUnit( const QString& unit );
    void setSliderInverted( bool inverted );


    double getValue();
    int getDecimals();
    double getMaximum();
    double getMinimum();
    double getDefaultValue();
    const QString getUnit();
    bool getSliderInverted();

  private slots:
    void on_slValue_valueChanged( int value );
    void on_dsbValue_valueChanged( double value );
    void on_pbValueReset_clicked();

  signals:
    void valueChanged( double );

  private:
    Ui::SliderDock* ui = nullptr;

    double defaultValue = 0;

  public:
    bool resetOnStart = false;
};
