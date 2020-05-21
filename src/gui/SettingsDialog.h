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

#include <QDialog>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include <QGraphicsScene>
#include <QSortFilterProxyModel>

#include "../block/PoseSynchroniser.h"

#include "NewOpenSaveToolbar.h"

#include "VectorBlockModel.h"
#include "NumberBlockModel.h"
#include "ActionDockBlockModel.h"
#include "SliderDockBlockModel.h"
#include "StringBlockModel.h"
#include "ImplementBlockModel.h"
#include "PathPlannerModelBlockModel.h"
#include "ImplementSectionModel.h"
#include "ValueBlockModel.h"
#include "FontComboboxDelegate.h"
#include "TransmissionBlockModel.h"

#include "../cgalKernel.h"

class SpaceNavigatorPollingThread;
class GeographicConvertionWrapper;

namespace Ui {
  class SettingsDialog;
}

class MyMainWindow;
namespace Qt3DExtras {
  class Qt3DWindow;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, MyMainWindow* mainWindow, Qt3DExtras::Qt3DWindow* qt3dWindow, QMenu* guidanceToolbarMenu, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

    void emitAllConfigSignals();

  signals:
    void setGrid( bool );
    void setGridValues( float, float, float, float, float, float, float, QColor, QColor );
    void plannerSettingsChanged( int, double );

    void globalPlannerModelSetVisible( bool );

    void globalPlannerModelSettingsChanged( int, int, float, int, int,
                                            QColor, QColor, QColor, QColor );

    void localPlannerModelSetVisible( bool );
    void localPlannerModelSettingsChanged( float, float,
                                           float,
                                           QColor, QColor );

  public slots:
    void toggleVisibility();

    void onStart();
    void onExit();

    void loadDefaultConfig();
    void saveDefaultConfig();

    void allModelsReset();

  private slots:
    void on_cbValues_currentIndexChanged( int index );
    void on_pbSetStringToFilename_clicked();

    void on_pbAddBlock_clicked();
    void on_pbSaveSelected_clicked();
    void on_pbLoad_clicked();

    void on_pbZoomOut_clicked();
    void on_pbZoomIn_clicked();
    void on_pbDeleteSelected_clicked();

    void on_gbGrid_toggled( bool arg1 );
    void on_dsbGridXStep_valueChanged( double arg1 );
    void on_dsbGridYStep_valueChanged( double arg1 );
    void on_dsbGridSize_valueChanged( double arg1 );
    void on_dsbGridXStepCoarse_valueChanged( double arg1 );
    void on_dsbGridYStepCoarse_valueChanged( double arg1 );
    void on_dsbGridCameraThreshold_valueChanged( double arg1 );
    void on_dsbGridCameraThresholdCoarse_valueChanged( double arg1 );
    void on_pbColor_clicked();
    void on_pbColorCoarse_clicked();

    void on_cbSaveConfigOnExit_stateChanged( int arg1 );
    void on_cbLoadConfigOnStart_stateChanged( int arg1 );
    void on_cbOpenSettingsDialogOnStart_stateChanged( int arg1 );
    void on_cbRunSimulatorOnStart_stateChanged( int arg1 );

    void on_pbSaveAsDefault_clicked();
    void on_pbLoadSavedConfig_clicked();
    void on_pbDeleteSettings_clicked();

    void on_pbComPortRefresh_clicked();
    void on_pbBaudrateRefresh_clicked();

    void on_pbComPortSet_clicked();
    void on_pbBaudrateSet_clicked();

    void on_pbClear_clicked();

    void on_cbImplements_currentIndexChanged( int index );

    void implementModelReset();
    void pathPlannerModelReset();

    void on_btnSectionAdd_clicked();
    void on_btnSectionRemove_clicked();

    void on_pbSetSelectedCellsToNumber_clicked();

    void on_btnSectionMoveUp_clicked();
    void on_btnSectionMoveDown_clicked();

    void on_sbPathsInReserve_valueChanged( int arg1 );
    void on_sbGlobalPlannerMaxDeviation_valueChanged( double );

    void on_cbRestoreDockPositions_toggled( bool checked );
    void on_cbSaveDockPositionsOnExit_toggled( bool checked );

    void on_pbMeterDefaults_clicked();

    void on_rbCrsSimulatorTransverseMercator_toggled( bool checked );
    void on_rbCrsGuidanceTransverseMercator_toggled( bool checked );

    void on_pbSaveAll_clicked();

    void on_pbSaveDockPositionsAsDefault_clicked();
    void on_pbSaveDockPositions_clicked();
    void on_pbLoadDockPositions_clicked();

    void on_rbMaterialAuto_clicked();
    void on_rbMaterialPRB_clicked();
    void on_rbMaterialPhong_clicked();

    void on_cbPathPlanner_currentIndexChanged( int index );

    void on_lePathPlannerName_textChanged( const QString& arg1 );
    void on_dsbPathlPlannerZOffset_valueChanged( double arg1 );
    void on_cbPathPlannerRayColor_stateChanged( int arg1 );
    void on_cbPathPlannerSegmentColor_stateChanged( int arg1 );
    void on_dsbPathlPlannerViewbox_valueChanged( double arg1 );
    void on_gbPathPlanner_toggled( bool arg1 );

    void on_pbPathPlannerLineColor_clicked();
    void on_pbPathPlannerRayColor_clicked();
    void on_pbPathPlannerSegmentColor_clicked();
    void on_pbPathPlannerBisectorsColor_clicked();

    void on_cbPathPlannerBisectors_stateChanged( int arg1 );

  private:
    void saveGridValuesInSettings();
    void savePathPlannerValuesInSettings();

    void setPathPlannerSettings();

    void saveConfigToFile( QFile& file );
    void loadConfigFromFile( QFile& file );

    void emitGridSettings();

  private:
    QMainWindow* mainWindow = nullptr;

  public:
    QComboBox* getCbNodeType();

  public:
    NewOpenSaveToolbar* newOpenSaveToolbar = nullptr;

    BlockBase* poseSimulation = nullptr;

    BlockBase* fieldManager = nullptr;
    BlockBase* plannerGui = nullptr;
    BlockBase* globalPlanner = nullptr;
    BlockBase* localPlanner = nullptr;
    BlockBase* stanleyGuidance = nullptr;
    BlockBase* xteGuidance = nullptr;
    BlockBase* globalPlannerModel = nullptr;

  private:
    Ui::SettingsDialog* ui = nullptr;

    GeographicConvertionWrapper* geographicConvertionWrapperGuidance = nullptr;
    GeographicConvertionWrapper* geographicConvertionWrapperSimulator = nullptr;

    BlockFactory* poseSimulationFactory = nullptr;

#ifdef SPNAV_ENABLED
    SpaceNavigatorPollingThread* spaceNavigatorPollingThread = nullptr;
#endif

    BlockFactory* transverseMercatorConverterFactory = nullptr;
    BlockFactory* poseSynchroniserFactory = nullptr;
    BlockFactory* tractorModelFactory = nullptr;
    BlockFactory* trailerModelFactory = nullptr;
    BlockFactory* sprayerModelFactory = nullptr;
    BlockFactory* fixedKinematicFactory = nullptr;
    BlockFactory* trailerKinematicFactory = nullptr;
    BlockFactory* debugSinkFactory = nullptr;
    BlockFactory* udpSocketFactory = nullptr;

#ifdef SERIALPORT_ENABLED
    BlockFactory* serialPortFactory = nullptr;
#endif

    BlockFactory* fileStreamFactory = nullptr;
    BlockFactory* ackermannSteeringFactory = nullptr;
    BlockFactory* angularVelocityLimiterFactory = nullptr;
    BlockFactory* ubxParserFactory = nullptr;
    BlockFactory* nmeaParserGGAFactory = nullptr;
    BlockFactory* nmeaParserHDTFactory = nullptr;
    BlockFactory* nmeaParserRMCFactory = nullptr;
    BlockFactory* communicationPgn7ffeFactory = nullptr;
    BlockFactory* communicationJrkFactory = nullptr;

    BlockFactory* fieldManagerFactory = nullptr;
    BlockFactory* plannerGuiFactory = nullptr;
    BlockFactory* globalPlannerFactory = nullptr;
    BlockFactory* localPlannerFactory = nullptr;
    BlockFactory* stanleyGuidanceFactory = nullptr;
    BlockFactory* xteGuidanceFactory = nullptr;
    BlockFactory* pathPlannerModelFactory = nullptr;

    BlockFactory* valueTransmissionBase64DataFactory = nullptr;
    BlockFactory* valueTransmissionNumberFactory = nullptr;
    BlockFactory* valueTransmissionQuaternionFactory = nullptr;
    BlockFactory* valueTransmissionStateFactory = nullptr;

    BlockFactory* vectorFactory = nullptr;
    BlockFactory* numberFactory = nullptr;
    BlockFactory* stringFactory = nullptr;

    QSortFilterProxyModel* filterModelValues = nullptr;
    VectorBlockModel* vectorBlockModel = nullptr;
    NumberBlockModel* numberBlockModel = nullptr;
    ActionDockBlockModel* actionBlockModel = nullptr;
    SliderDockBlockModel* sliderBlockModel = nullptr;
    StringBlockModel* stringBlockModel = nullptr;

    QSortFilterProxyModel* filterModelImplements = nullptr;
    QSortFilterProxyModel* filterModelPathPlannerModel = nullptr;

  public:
    ImplementBlockModel* implementBlockModel = nullptr;
    PathPlannerModelBlockModel* pathPlannerModelBlockModel = nullptr;

  private:
    ImplementSectionModel* implementSectionModel = nullptr;

    ValueBlockModel* meterModel = nullptr;
    QSortFilterProxyModel* filterModelMeter = nullptr;
    FontComboboxDelegate* meterModelFontDelegate = nullptr;

    TransmissionBlockModel* transmissionBlockModel = nullptr;
    QSortFilterProxyModel* filterTtransmissionBlockModel = nullptr;

  private:
    QNEBlock* getBlockWithId( int id );
    QNEBlock* getBlockWithName( const QString& name );

    bool blockSettingsSaving = false;

    QColor gridColor = QColor( 0x6b, 0x96, 0xa8 );
    QColor gridColorCoarse = QColor( 0xa2, 0xe3, 0xff );

    QColor globalPlannerArrowColor = QColor( 0xff, 0xff, 0 );
    QColor globalPlannerCenterLineColor = QColor( 0xff, 0x00, 0 );
    QColor globalPlannerBorderLineColor = QColor( 0x99, 0x99, 0 );
    QColor globalPlannerBackgroundColor = QColor( 0xf5, 0x9f, 0xbd );

    QColor localPlannerArrowColor = QColor( 0x90, 0x90, 0 );
    QColor localPlannerLineColor = QColor( 0x9a, 0x64, 0x77 );
};
