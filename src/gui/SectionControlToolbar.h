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

#include <QWidget>

#include <QGroupBox>

class Implement;
class QGridLayout;
class QLabel;
class QToolButton;

class SectionControlToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit SectionControlToolbar( Implement* implement, QWidget* parent = nullptr );

  Q_SIGNALS:

  public Q_SLOTS:
    void sectionsChanged();
    void implementChanged( const QPointer<Implement>& );
    void setDockLocation( const Qt::DockWidgetArea area );

  private Q_SLOTS:
    void forceOnOffToggled( const bool );
    void autoToggled( const bool );

  private:
    QToolButton* addButtonToVector( const QString& name );
    void addSection( const QString& name );

  private:
    Implement* implement = nullptr;
    QPalette buttonYellow, buttonRed, buttonGreen, buttonDefault;

    QGridLayout* gridLayout = nullptr;

    QLabel* lbOn = nullptr;
    QLabel* lbOff = nullptr;
    QToolButton* pbAuto = nullptr;
    std::vector<QToolButton*> buttons;

    bool horizontal = true;
};
