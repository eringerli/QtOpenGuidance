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

#ifndef GUIDANCETURNING_H
#define GUIDANCETURNING_H

#include <QGroupBox>
#include <QHBoxLayout>

namespace Ui {
  class GuidanceTurning;
}

class GuidanceTurning : public QGroupBox {
    Q_OBJECT

  public:
    explicit GuidanceTurning( QWidget* parent = nullptr );
    ~GuidanceTurning();

  private slots:
    void on_pbLeft_clicked();
    void on_pbRight_clicked();

  signals:
    void turnLeft();
    void turnRight();

  private:
    Ui::GuidanceTurning* ui = nullptr;
};

#endif // GUIDANCETURNING_H
