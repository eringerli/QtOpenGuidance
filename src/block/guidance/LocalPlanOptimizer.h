// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>

#include "block/BlockBase.h"
#include "block/kinematic/FixedKinematicPrimitive.h"

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

class LocalPlanOptimizer : public BlockBase {
  Q_OBJECT

public:
  explicit LocalPlanOptimizer( const QString& uniqueName, const int idHint, const bool systemBlock, const QString type )
      : BlockBase( idHint, systemBlock, type ) {}

public Q_SLOTS:
  void setPoseTractor( POSE_SIGNATURE_SLOT );
  void setPoseTrailer( POSE_SIGNATURE_SLOT );

  void setPlan( const Plan& plan );

Q_SIGNALS:
  void planChanged( const Plan& );

  void triggerPlanPose( POSE_SIGNATURE_SIGNAL );

public:
  Eigen::Vector3d positionTractor = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Vector3d positionTrailer = Eigen::Vector3d( 0, 0, 0 );

  PathPlannerModel* pathPlannerModel = nullptr;

private:
  Plan                         localPlan;
  Plan::ConstPrimitiveIterator lastNearestPrimitiveLocalPlan;
  Plan::PrimitiveSharedPointer lastPrimitiveLocalPlan = nullptr;

  Plan::PrimitiveSharedPointer targetPrimitiveGlobalPlan = nullptr;

  Plan optimizedPlan;

  FixedKinematicPrimitive tractorToTrailerHalf = FixedKinematicPrimitive( 0, false, "FixedKinematicPrimitive" );
};

class LocalPlanOptimizerFactory : public BlockFactory {
  Q_OBJECT

public:
  LocalPlanOptimizerFactory(
    QThread* thread, MyMainWindow* mainWindow, KDDockWidgets::Location location, QMenu* menu, Qt3DCore::QEntity* rootEntity )
      : BlockFactory( thread, false ), mainWindow( mainWindow ), location( location ), menu( menu ), rootEntity( rootEntity ) {
    typeColor = TypeColor::Arithmetic;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Local Plan Optimizer" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Guidance" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;

private:
  MyMainWindow*           mainWindow = nullptr;
  KDDockWidgets::Location location;
  QMenu*                  menu       = nullptr;
  Qt3DCore::QEntity*      rootEntity = nullptr;
};
