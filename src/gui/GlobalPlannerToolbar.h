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
  class GlobalPlannerToolbar;
}

class GlobalPlannerToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit GlobalPlannerToolbar( QWidget* parent = nullptr );
    ~GlobalPlannerToolbar();
    void resetToolbar();
    void setToolbarToAdditionalPoint();

  private slots:
    void on_pbAB_clicked( bool checked );
    void on_pbSnap_clicked();

    void on_pbAbContinuous_clicked( bool checked );

  signals:
    void setAPoint();
    void setBPoint();
    void setAdditionalPoint();
    void setAdditionalPointsContinous( bool );
    void snap();

  private:
    Ui::GlobalPlannerToolbar* ui;
};
