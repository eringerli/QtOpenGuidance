// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class MyMainWindow;
class BufferMesh;
class GuidanceTurningToolbar;

class PathPlannerModel;

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

#include "kinematic/Plan.h"

#include "3d/qt3dForwards.h"

#include "job/PlanOptimitionController.h"

class LocalPlanner : public BlockBase {
  Q_OBJECT

public:
  explicit LocalPlanner( const QString& uniqueName, MyMainWindow* mainWindow );

  ~LocalPlanner();

public Q_SLOTS:
  void setName( const QString& name ) override;

  void setPose( POSE_SIGNATURE_SLOT );

  void setPlan( const Plan& plan );

  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setPathHysteresis( NUMBER_SIGNATURE_SLOT );

  void setMinRadius( NUMBER_SIGNATURE_SLOT );

  void setForceCurrentPath( ACTION_SIGNATURE_SLOT );

  void turnLeftToggled( const bool state );
  void turnRightToggled( const bool state );
  void numSkipChanged( const int left, const int right );

Q_SIGNALS:
  void planChanged( const Plan& );

  void triggerPlanPose( POSE_SIGNATURE_SIGNAL );
  void passNumberChanged( NUMBER_SIGNATURE_SIGNAL );

  void resetTurningStateOfDock();

public:
  Eigen::Vector3d    position             = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientation          = Eigen::Quaterniond( 0, 0, 0, 0 );
  double             steeringAngleDegrees = 0;
  double             pathHysteresis       = 0.5;
  double             minRadius            = 10;
  bool               forceCurrentPath     = false;

  GuidanceTurningToolbar*    widget           = nullptr;
  KDDockWidgets::DockWidget* dock             = nullptr;
  PathPlannerModel*          pathPlannerModel = nullptr;

private:
  void calculateTurning( bool changeExistingTurn );

  Plan                         globalPlan;
  Plan::ConstPrimitiveIterator lastNearestPrimitiveGlobalPlan;
  Plan::PrimitiveSharedPointer lastPrimitiveGlobalPlan = nullptr;

  Plan::PrimitiveSharedPointer targetPrimitiveGlobalPlan = nullptr;

  Plan localPlan;

  bool    turningLeft       = false;
  bool    turningRight      = false;
  int     leftSkip          = 1;
  int     rightSkip         = 1;
  Point_2 positionTurnStart = Point_2( 0, 0 );
  double  headingTurnStart  = 0;

  QPointer< PlanOptimitionController > planOptimitionController;
};

class LocalPlannerFactory : public BlockFactory {
  Q_OBJECT

public:
  LocalPlannerFactory(
    QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, Qt3DCore::QEntity* rootEntity )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ), rootEntity( rootEntity ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Local Planner" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu       = nullptr;
  Qt3DCore::QEntity*      rootEntity = nullptr;
};
