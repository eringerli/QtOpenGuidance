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

#ifndef SECTIONCONTROLTOOLBAR_H
#define SECTIONCONTROLTOOLBAR_H

#include <QWidget>

#include <QGroupBox>
#include <QToolButton>
#include <QLabel>
#include <QPalette>
#include <QGridLayout>
#include <QDockWidget>

#include <QVector>

class Implement;

class SectionControlToolbar : public QGroupBox {
    Q_OBJECT

  public:
    explicit SectionControlToolbar( Implement* implement, QWidget* parent = nullptr );

  signals:

  public slots:
    void implementChanged();
    void sectionsChanged();
    void setDockLocation( Qt::DockWidgetArea area );

  private slots:
    void forceOnOffToggled( bool );
    void autoToggled( bool );

  private:
    QToolButton* addButtonToVector( QString name );
    void addSection( QString name );

  private:
    Implement* implement = nullptr;
    QPalette buttonYellow, buttonRed, buttonGreen, buttonDefault;

    QGridLayout* gridLayout = nullptr;

    QLabel* lbOn = nullptr;
    QLabel* lbOff = nullptr;
    QToolButton* pbAuto = nullptr;
    QVector<QToolButton*> buttons;

    bool horizontal = true;
};

#endif // SECTIONCONTROLTOOLBAR_H
