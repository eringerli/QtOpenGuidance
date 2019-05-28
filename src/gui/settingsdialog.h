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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include <QGraphicsScene>

#include "../PoseCache.h"

#include "VectorBlockModel.h"

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

  signals:
    void antennaPositionChanged( QVector3D );
    void wheelbaseChanged( float );
    void hitchPositionChanged( QVector3D );
    void cameraChanged( int );

  public slots:
    void toggleVisibility();


  private slots:
    void on_rb_fixedCamera_clicked();
    void on_rb_firstPersonCamera_clicked();
    void on_rb_orbitCamera_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_cbValues_currentIndexChanged( int index );

  private:
    Ui::SettingsDialog* ui;

    Qt3DCore::QEntity* rootEntity;

    GuidanceFactory* poseCacheFactory;
    GuidanceFactory* tractorModelFactory;
    GuidanceFactory* trailerModelFactory;
    GuidanceFactory* fixedKinematicFactory;
    GuidanceFactory* trailerKinematicFactory;
    GuidanceFactory* vectorFactory;
    GuidanceFactory* lengthFactory;

    VectorBlockModel* vectorBlockModel;
};

#endif // SETTINGSDIALOG_H
