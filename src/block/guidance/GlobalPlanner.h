// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>

#include "block/BlockBase.h"

#include "kinematic/Plan.h"
#include "kinematic/PlanGlobal.h"

class MyMainWindow;
class FieldsOptimitionToolbar;
class GlobalPlannerToolbar;
class BufferMesh;
class GeographicConvertionWrapper;
class Plan;
class PlanGlobal;
class OpenSaveHelper;
class PathPlannerModel;

namespace Qt3DCore {
  class QEntity;
}

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include <utility>

#include "job/PlanOptimitionController.h"

class GlobalPlanner : public BlockBase {
  Q_OBJECT

public:
  explicit GlobalPlanner( const QString& uniqueName, MyMainWindow* mainWindow, GeographicConvertionWrapper* tmw );

  ~GlobalPlanner();

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );
  void setPoseLeftEdge( POSE_SIGNATURE_SLOT );
  void setPoseRightEdge( POSE_SIGNATURE_SLOT );

  void setField( std::shared_ptr< Polygon_with_holes_2 > field );

  void aPointSet();

  void bPointSet();

  void additionalPointSet();

  void additionalPointsContinousSet( ACTION_SIGNATURE_SLOT );

  void snap();

  void openAbLineFromString( const QString& fileName );
  void openAbLineFromFile( QFile& file );

  void newAbLine();

  void saveAbLineToString( QString fileName );
  void saveAbLineToFile( QFile& file );

  void setPlannerSettings( const int pathsInReserve, const double maxDeviation );

  void setPassSettings( const int forwardPasses, const int reversePasses, const bool startRight, const bool mirror );

  void setPassNumberTo( const int /*passNumber*/ );

  void createPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polylinePtr );

Q_SIGNALS:
  void planChanged( const Plan& );

  void planChangedForModel( const Plan& );
  void planPolylineChanged( std::shared_ptr< std::vector< Point_2 > > );

  void setToolbarToAdditionalPoint();
  void resetToolbar();

private:
  void createPlanAB();
  void snapPlanAB();

public:
  Point_3            position    = Point_3( 0, 0, 0 );
  Eigen::Quaterniond orientation = Eigen::Quaterniond( 0, 0, 0, 0 );

  int  forwardPasses = 0;
  int  reversePasses = 0;
  bool startRight    = false;
  bool mirror        = false;

  double maxDeviation = 0.1;

  std::shared_ptr< Polygon_with_holes_2 > currentField;

  Point_3                                   aPoint    = Point_3( 0, 0, 0 );
  Point_3                                   bPoint    = Point_3( 0, 0, 0 );
  Segment_3                                 abSegment = Segment_3( Point_3( 0, 0, 0 ), Point_3( 0, 0, 0 ) );
  std::shared_ptr< std::vector< Point_2 > > abPolyline;
  bool                                      recordContinous = false;

  Segment_2 implementSegment = Segment_2( Point_2( 0, 0 ), Point_2( 0, 0 ) );

  Point_3 positionLeftEdgeOfImplement  = Point_3( 0, 0, 0 );
  Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

  PlanGlobal plan = PlanGlobal();

  MyMainWindow*              mainWindow       = nullptr;
  GlobalPlannerToolbar*      widget           = nullptr;
  KDDockWidgets::DockWidget* dock             = nullptr;
  PathPlannerModel*          pathPlannerModel = nullptr;

  GeographicConvertionWrapper* tmw = nullptr;

  OpenSaveHelper* openSaveHelper = nullptr;

private:
  QPointer< PlanOptimitionController > planOptimitionController;
  Plan                                 planForModel = Plan();
  Plan::ConstPrimitiveIterator         lastNearestPrimitive;
};

class GlobalPlannerFactory : public BlockFactory {
  Q_OBJECT

public:
  GlobalPlannerFactory( QThread*                     thread,
                        MyMainWindow*                mainWindow,
                        KDDockWidgets::Location      location,
                        QMenu*                       menu,
                        GeographicConvertionWrapper* tmw,
                        Qt3DCore::QEntity*           rootEntity );

  QString getNameOfFactory() override { return QStringLiteral( "Global Planner Lines" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  MyMainWindow*                mainWindow = nullptr;
  KDDockWidgets::Location      location;
  QMenu*                       menu       = nullptr;
  GeographicConvertionWrapper* tmw        = nullptr;
  Qt3DCore::QEntity*           rootEntity = nullptr;
};
