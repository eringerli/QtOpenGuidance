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

#ifndef GUIDANCETOOLBARTOP_H
#define GUIDANCETOOLBARTOP_H

#include <QGroupBox>
#include <QHBoxLayout>

namespace Ui {
  class GuidanceToolbarTop;
}

class GuidanceToolbarTop : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceToolbarTop( QWidget* parent = nullptr );
    ~GuidanceToolbarTop();

  private slots:
    void on_pbLeft_clicked();
    void on_pbRight_clicked();

  signals:
    void turnLeft();
    void turnRight();

  private:
    Ui::GuidanceToolbarTop* ui;
};

#endif // GUIDANCETOOLBARTOP_H
