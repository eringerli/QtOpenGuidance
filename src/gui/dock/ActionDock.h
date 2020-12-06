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
  class ActionDock;
}

class ActionDock : public QGroupBox {
    Q_OBJECT

  public:
    explicit ActionDock( QWidget* parent = nullptr );
    ~ActionDock();

    bool state();
    bool isCheckable();
    const QString getTheme();

    void setState( bool state );
    void setCheckable( bool checkable );
    void setTheme( const QString theme );

  private slots:
    void on_pbAction_clicked( bool checked );

  signals:
    void action( bool );

  private:
    QString theme;
    Ui::ActionDock* ui;
};
