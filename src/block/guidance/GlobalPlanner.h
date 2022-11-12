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
#include <QPointer>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

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

#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/KDDockWidgets.h>

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include <utility>

#include "job/PlanOptimitionController.h"

class GlobalPlanner : public BlockBase {
  Q_OBJECT

public:
  explicit GlobalPlanner( const QString&               uniqueName,
                          MyMainWindow*                mainWindow,
                          GeographicConvertionWrapper* tmw,
                          Qt3DCore::QEntity*           rootEntity );

  ~GlobalPlanner();

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );
  void setPoseLeftEdge( POSE_SIGNATURE_SLOT );
  void setPoseRightEdge( POSE_SIGNATURE_SLOT );

  void setField( std::shared_ptr< Polygon_with_holes_2 > field );

  void setAPoint();

  void setBPoint();

  void setAdditionalPoint();

  void setAdditionalPointsContinous( ACTION_SIGNATURE_SLOT );

  void snap();

  void openAbLine();
  void openAbLineFromFile( QFile& file );

  void newField();

  void saveAbLine();
  void saveAbLineToFile( QFile& file );

  void setPlannerSettings( const int pathsInReserve, const double maxDeviation );

  void setPassSettings( const int forwardPasses, const int reversePasses, const bool startRight, const bool mirror );

  void setPassNumberTo( const int /*passNumber*/ );

  void createPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polylinePtr );

Q_SIGNALS:
  void planChanged( const Plan& );

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

  GlobalPlannerToolbar*      widget = nullptr;
  KDDockWidgets::DockWidget* dock   = nullptr;

  GeographicConvertionWrapper* tmw = nullptr;

private:
  QWidget*           mainWindow = nullptr;
  Qt3DCore::QEntity* rootEntity = nullptr;

  QPointer< PlanOptimitionController > planOptimitionController;

  // markers
  Qt3DCore::QEntity*       aPointEntity    = nullptr;
  Qt3DExtras::QSphereMesh* aPointMesh      = nullptr;
  Qt3DCore::QTransform*    aPointTransform = nullptr;
  Qt3DCore::QEntity*       aTextEntity     = nullptr;
  Qt3DCore::QTransform*    aTextTransform  = nullptr;

  Qt3DCore::QEntity*       bPointEntity    = nullptr;
  Qt3DExtras::QSphereMesh* bPointMesh      = nullptr;
  Qt3DCore::QTransform*    bPointTransform = nullptr;
  Qt3DCore::QEntity*       bTextEntity     = nullptr;
  Qt3DCore::QTransform*    bTextTransform  = nullptr;

  Qt3DCore::QEntity*          pointsEntity   = nullptr;
  Qt3DExtras::QSphereMesh*    pointsMesh     = nullptr;
  Qt3DExtras::QPhongMaterial* pointsMaterial = nullptr;

private:
  Qt3DCore::QEntity*    m_baseEntity    = nullptr;
  Qt3DCore::QTransform* m_baseTransform = nullptr;
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
  Qt3DCore::QEntity*           rootEntity = nullptr;
  GeographicConvertionWrapper* tmw        = nullptr;
};
