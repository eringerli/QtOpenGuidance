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

#include "VectorBlockModel.h"
#include "NumberBlockModel.h"
#include "SliderBlockModel.h"
#include "StringBlockModel.h"
#include "ImplementBlockModel.h"
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

class SettingsDialog : public QDialog {
    Q_OBJECT

  public:
    explicit SettingsDialog( Qt3DCore::QEntity* rootEntity, MyMainWindow* mainWindow, QMenu* menu, QWidget* parent = nullptr );
    ~SettingsDialog();

    QGraphicsScene* getSceneOfConfigGraphicsView();

    void emitAllConfigSignals();

  signals:
    void setGrid( bool );
    void setGridValues( float, float, float, float, float, float, float, QColor, QColor );
    void plannerSettingsChanged( int, int );

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

    void on_btnSectionAdd_clicked();

    void on_btnSectionRemove_clicked();

    void on_pbSetSelectedCellsToNumber_clicked();

    void on_btnSectionMoveUp_clicked();

    void on_btnSectionMoveDown_clicked();

    void on_gbGlobalPlanner_toggled( bool arg1 );
    void on_pbGlobalPlannerArrowColor_clicked();
    void on_pbGlobalPlannerBackgroundColor_clicked();
    void on_slGlobalPlannerTransparency_valueChanged( int value );
    void on_cbGlobalPlannerBackground_stateChanged( int );
    void on_pbGlobalPlannerCenterLineColor_clicked();
    void on_pbGlobalPlannerBorderLineColor_clicked();
    void on_slGlobalPlannerArrowSize_valueChanged( int );
    void on_slGlobalPlannerCenterLine_valueChanged( int );
    void on_slGlobalPlannerBorderLine_valueChanged( int );
    void on_dsbGlobalPlannerTextureSize_valueChanged( double );
    void on_slGlobalPlannerArrowWidth_valueChanged( int );

    void on_gbLocalPlanner_toggled( bool arg1 );
    void on_dsbLocalPlannerArrowSize_valueChanged( double arg1 );
    void on_dsbLocalPlannerLineWidth_valueChanged( double arg1 );
    void on_pbLocalPlannerArrowColor_clicked();
    void on_pbLocalPlannerLineColor_clicked();
    void on_slLocalPlannerTransparency_valueChanged( int value );

    void on_sbPathsToGenerate_valueChanged( int arg1 );

    void on_sbPathsInReserve_valueChanged( int arg1 );

    void on_cbRestoreDockPositions_toggled( bool checked );
    void on_cbSaveDockPositionsOnExit_toggled( bool checked );
    void on_pbSaveDockPositions_clicked();

    void on_pbMeterDefaults_clicked();

    void on_rbCrsSimulatorTransverseMercator_toggled( bool checked );
    void on_rbCrsGuidanceTransverseMercator_toggled( bool checked );

    void on_pbSaveAll_clicked();

  private:
    void saveGridValuesInSettings();
    void savePlannerValuesInSettings();
    void savePathPlannerValuesInSettings();

    void setPlannerColorLabels();

    void saveConfigToFile( QFile& file );
    void loadConfigFromFile( QFile& file );

    void emitGridSettings();
    void emitGlobalPlannerModelSettings();
    void emitLocalPlannerModelSettings();

  private:
    QMainWindow* mainWindow = nullptr;

  public:
    QComboBox* getCbNodeType();

  public:
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
    BlockFactory* printLatencyFactory = nullptr;
    BlockFactory* udpSocketFactory = nullptr;

#ifdef SERIALPORT_ENABLED
    BlockFactory* serialPortFactory = nullptr;
#endif

    BlockFactory* fileStreamFactory = nullptr;
    BlockFactory* ackermannSteeringFactory = nullptr;
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
    BlockFactory* globalPlannerModelFactory = nullptr;

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
    SliderBlockModel* sliderBlockModel = nullptr;
    StringBlockModel* stringBlockModel = nullptr;

    QSortFilterProxyModel* filterModelImplements = nullptr;
  public:
    ImplementBlockModel* implementBlockModel = nullptr;
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

