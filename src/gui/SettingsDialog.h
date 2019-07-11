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

#include <QObject>

#include <QDialog>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include <QGraphicsScene>
#include <QSortFilterProxyModel>

#include "../block/PoseSynchroniser.h"

#include "VectorBlockModel.h"
#include "NumberBlockModel.h"
#include "StringBlockModel.h"

#include "../kinematic/Tile.h"

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

    void emitAllConfigSignals();

  signals:
    void setGrid( bool );
    void setGridValues( float, float, float, QColor );

  public slots:
    void toggleVisibility();

    void saveConfigOnExit();


  private slots:
    void on_cbValues_currentIndexChanged( int index );

    void on_pbLoad_clicked();

    void on_pbSaveSelected_clicked();

    void on_pbAddBlock_clicked();

    void on_pbZoomOut_clicked();

    void on_pbZoomIn_clicked();

    void on_pbDeleteSelected_clicked();

    void on_gbGrid_toggled( bool arg1 );
    void on_dsbGridXStep_valueChanged( double arg1 );
    void on_dsbGridYStep_valueChanged( double arg1 );
    void on_dsbGridSize_valueChanged( double arg1 );

    void on_pbColor_clicked();

    void on_cbSaveConfigOnExit_stateChanged( int arg1 );
    void on_cbLoadConfigOnStart_stateChanged( int arg1 );
    void on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 );
    void on_cbShowCameraToolbarOnStart_stateChanged( int arg1 );
    void on_cbRunSimulatorOnStart_stateChanged( int arg1 );

    void on_pbDeleteSettings_clicked();

  private:
    void saveGridValuesInSettings();

    void saveConfigToFile( QFile& file );
    void loadConfigFromFile( QFile& file );

  public:
    QComboBox* getCbNodeType();

    RootTile tileRoot;

  public:
    GuidanceBase* poseSimulation;
    GuidanceBase* gridModel;

  private:
    Ui::SettingsDialog* ui;

    Qt3DCore::QEntity* rootEntity;

    GuidanceFactory* poseSimulationFactory;
    GuidanceFactory* gridModelFactory;

    GuidanceFactory* transverseMercatorConverterFactory;
    GuidanceFactory* poseCacheFactory;
    GuidanceFactory* tractorModelFactory;
    GuidanceFactory* trailerModelFactory;
    GuidanceFactory* fixedKinematicFactory;
    GuidanceFactory* trailerKinematicFactory;
    GuidanceFactory* vectorFactory;
    GuidanceFactory* numberFactory;
    GuidanceFactory* stringFactory;
    GuidanceFactory* debugSinkFactory;
    GuidanceFactory* printLatencyFactory;
    GuidanceFactory* udpSocketFactory;
    GuidanceFactory* serialPortFactory;
    GuidanceFactory* fileStreamFactory;

    QSortFilterProxyModel* filterModel;
    VectorBlockModel* vectorBlockModel;
    NumberBlockModel* numberBlockModel;
    StringBlockModel* stringBlockModel;

//    GuidanceFactory* fieldFactory;


    QNEBlock* getBlockWithId( int id );

    QColor gridColor = QColor( 0xa2, 0xe3, 0xff );
};

#endif // SETTINGSDIALOG_H
