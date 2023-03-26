// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QColor>
#include <QDialog>

class VectorBlockModel;
class OrientationBlockModel;
class NumberBlockModel;
class ActionDockBlockModel;
class SliderDockBlockModel;
class StringBlockModel;
class ImplementBlockModel;
class PathPlannerModelBlockModel;
class ImplementSectionModel;
class ValueBlockModel;
class TransmissionBlockModel;
class PlotBlockModel;
class FontComboboxDelegate;

class QGraphicsScene;
class QSortFilterProxyModel;
class QTreeWidget;
class QTreeWidgetItem;
class QFile;
class QMainWindow;
class MyMainWindow;

class TerrainModel;

class QNEBlock;

#include "block/BlockBase.h"

class NewOpenSaveToolbar;

#include "3d/qt3dForwards.h"

#include "helpers/cgalHelper.h"

class SpaceNavigatorPollingThread;
class SdlInputPollingThread;
class GeographicConvertionWrapper;

namespace Ui {
  class SettingsDialog;
}

class SettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit SettingsDialog( Qt3DCore::QEntity*      foregroundEntity,
                           Qt3DCore::QEntity*      middlegroundEntity,
                           Qt3DCore::QEntity*      backgroundEntity,
                           MyMainWindow*           mainWindow,
                           Qt3DExtras::Qt3DWindow* qt3dWindow,
                           QMenu*                  guidanceToolbarMenu,
                           QThread*                calculationsThread,
                           QWidget*                parent = nullptr );
  ~SettingsDialog();

  QGraphicsScene* getSceneOfConfigGraphicsView();

  void emitAllConfigSignals();

Q_SIGNALS:
  void setGrid( const bool );
  void
  setGridValues( const float, const float, const float, const float, const float, const float, const float, const QColor, const QColor );
  void plannerSettingsChanged( const int, const double );

  void globalPlannerModelSetVisible( const bool );

  void globalPlannerModelSettingsChanged(
    const int, const int, const float, const int, const int, const QColor, const QColor, const QColor, const QColor );

  void localPlannerModelSetVisible( const bool );
  void localPlannerModelSettingsChanged( const float, const float, const float, const QColor, const QColor );

  void noiseStandartDeviationsChanged( const double, const double, const double, const double, const double );

  void cameraSmoothingChanged( const int, const int );

  void simulatorValuesChanged( const double a,
                               const double b,
                               const double c,
                               const double Caf,
                               const double Car,
                               const double Cah,
                               const double m,
                               const double Iz,
                               const double sigmaF,
                               const double sigmaR,
                               const double sigmaH,
                               const double Cx,
                               const double slip,
                               const double slewRateAutosteerSteering );
public Q_SLOTS:
  void toggleVisibility();

  void onStart();
  void onExit();

  void loadDefaultConfig();
  void saveDefaultConfig();

  void resetAllModels();

  void setSimulatorValues( const double a,
                           const double b,
                           const double c,
                           const double Caf,
                           const double Car,
                           const double Cah,
                           const double m,
                           const double Iz,
                           const double sigmaF,
                           const double sigmaR,
                           const double sigmaH,
                           const double Cx,
                           const double slipX,
                           const double slewRateAutosteerSteering );

private Q_SLOTS:
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
  void on_cbPathPlannerArcColor_stateChanged( int arg1 );
  void on_cbPathPlannerRayColor_stateChanged( int arg1 );
  void on_cbPathPlannerSegmentColor_stateChanged( int arg1 );
  void on_dsbPathlPlannerViewbox_valueChanged( double arg1 );
  void on_gbPathPlanner_toggled( bool arg1 );

  void on_pbPathPlannerArcColor_clicked();
  void on_pbPathPlannerLineColor_clicked();
  void on_pbPathPlannerRayColor_clicked();
  void on_pbPathPlannerSegmentColor_clicked();
  void on_pbPathPlannerBisectorsColor_clicked();

  void on_cbPathPlannerBisectors_stateChanged( int arg1 );

  void on_dsbGamma_valueChanged( double arg1 );

  void on_cbShowDebugOverlay_toggled( bool checked );

  void on_pbGammaDefault_clicked();

  void on_pbPlotsDefaults_clicked();

  void on_dsbSimNoisePositionXY_valueChanged( double arg1 );
  void on_dsbSimNoiseGyro_valueChanged( double arg1 );
  void on_dsbSimNoisePositionZ_valueChanged( double arg1 );
  void on_dsbSimNoiseOrientation_valueChanged( double arg1 );
  void on_dsbSimNoiseAccelerometer_valueChanged( double arg1 );

  void on_slCameraSmoothingOrientation_valueChanged( int value );
  void on_slCameraSmoothingPosition_valueChanged( int value );

  void on_twBlocks_itemDoubleClicked( QTreeWidgetItem* item, int column );

  void on_dsbSimGeneralSlipX_valueChanged( double arg1 );
  void on_dsbSimGeneralA_valueChanged( double arg1 );
  void on_dsbSimGeneralB_valueChanged( double arg1 );
  void on_dsbSimGeneralC_valueChanged( double arg1 );
  void on_dsbSimGeneralSigmaF_valueChanged( double arg1 );
  void on_dsbSimGeneralSigmaR_valueChanged( double arg1 );
  void on_dsbSimGeneralSigmaH_valueChanged( double arg1 );
  void on_dsbSimGeneralCaf_valueChanged( double arg1 );
  void on_dsbSimGeneralCar_valueChanged( double arg1 );
  void on_dsbSimGeneralCah_valueChanged( double arg1 );
  void on_dsbSimGeneralM_valueChanged( double arg1 );
  void on_dsbSimGeneralIz_valueChanged( double arg1 );
  void on_dsbSimGeneralCx_valueChanged( double arg1 );
  void on_dsbSimGeneralSlewrateAutoSteering_valueChanged( double arg1 );

private:
  void saveGridValuesInSettings();
  void savePathPlannerValuesInSettings();

  void setPathPlannerSettings();

  void saveConfigToFile( QFile& file );
  void loadConfigFromFile( QFile& file );

  void emitGridSettings();

  void emitSimulatorValues();
  void emitNoiseStandartDeviations();

private:
  QMainWindow*            mainWindow = nullptr;
  Qt3DExtras::Qt3DWindow* qt3dWindow = nullptr;

public:
  QTreeWidget* getBlockTreeWidget();

public:
  NewOpenSaveToolbar* newOpenSaveToolbar = nullptr;

  BlockBase* poseSimulation = nullptr;

  BlockBase* fieldManager  = nullptr;
  BlockBase* globalPlanner = nullptr;

private:
  Ui::SettingsDialog* ui = nullptr;

  GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;

  BlockFactory* poseSimulationFactory = nullptr;

#ifdef SPNAV_ENABLED
  SpaceNavigatorPollingThread* spaceNavigatorPollingThread = nullptr;
#endif

#ifdef SDL2_ENABLED
  SdlInputPollingThread* sdlInputPollingThread = nullptr;
#endif

  std::vector< std::unique_ptr< BlockFactory > > factories;

  QSortFilterProxyModel* filterModelValues     = nullptr;
  VectorBlockModel*      vectorBlockModel      = nullptr;
  OrientationBlockModel* orientationBlockModel = nullptr;
  NumberBlockModel*      numberBlockModel      = nullptr;
  ActionDockBlockModel*  actionBlockModel      = nullptr;
  SliderDockBlockModel*  sliderBlockModel      = nullptr;
  StringBlockModel*      stringBlockModel      = nullptr;

  QSortFilterProxyModel* filterModelImplements       = nullptr;
  QSortFilterProxyModel* filterModelPathPlannerModel = nullptr;

public:
  ImplementBlockModel*        implementBlockModel        = nullptr;
  PathPlannerModelBlockModel* pathPlannerModelBlockModel = nullptr;

private:
  ImplementSectionModel* implementSectionModel = nullptr;

  ValueBlockModel*       meterModel             = nullptr;
  QSortFilterProxyModel* filterModelMeter       = nullptr;
  FontComboboxDelegate*  meterModelFontDelegate = nullptr;

  PlotBlockModel*        plotBlockModel  = nullptr;
  QSortFilterProxyModel* filterModelPlot = nullptr;

  TransmissionBlockModel* transmissionBlockModel        = nullptr;
  QSortFilterProxyModel*  filterTtransmissionBlockModel = nullptr;

private:
  QNEBlock* getBlockWithId( int id );
  QNEBlock* getBlockWithName( const QString& name );

  bool blockSettingsSaving = false;

  QColor gridColor       = QColor( 0x6b, 0x96, 0xa8 );
  QColor gridColorCoarse = QColor( 0xa2, 0xe3, 0xff );

  QColor globalPlannerArrowColor      = QColor( 0xff, 0xff, 0 );
  QColor globalPlannerCenterLineColor = QColor( 0xff, 0x00, 0 );
  QColor globalPlannerBorderLineColor = QColor( 0x99, 0x99, 0 );
  QColor globalPlannerBackgroundColor = QColor( 0xf5, 0x9f, 0xbd );

  QColor localPlannerArrowColor = QColor( 0x90, 0x90, 0 );
  QColor localPlannerLineColor  = QColor( 0x9a, 0x64, 0x77 );
};
