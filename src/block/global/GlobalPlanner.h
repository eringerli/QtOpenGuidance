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

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <QDebug>

#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "3d/BufferMesh.h"

#include "gui/FieldsOptimitionToolbar.h"
#include "gui/GlobalPlannerToolbar.h"

#include "gui/MyMainWindow.h"
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include "kinematic/PlanGlobal.h"

#include "helpers/GeographicConvertionWrapper.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class CgalThread;
class CgalWorker;

class GlobalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlanner( const QString& uniqueName,
                            MyMainWindow* mainWindow,
                            GeographicConvertionWrapper* tmw,
                            Qt3DCore::QEntity* rootEntity );

    ~GlobalPlanner() {
      dock->deleteLater();
      widget->deleteLater();
    }

  public Q_SLOTS:
    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = toPoint3( position );
        this->orientation = orientation;

        auto quat = toQQuaternion( orientation );
        aPointTransform->setRotation( quat );
        bPointTransform->setRotation( quat );

        auto position2D = to2D( position );

        if( recordContinous ) {
          abPolyline.push_back( position2D );
        }

//        QElapsedTimer timer;
//        timer.start();
        plan.expand( position2D );
//        qDebug() << "Cycle Time plan.expandPlan:" << timer.nsecsElapsed() << "ns";
      }
    }

    void setPoseLeftEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = toPoint3( position );

        auto point2D = to2D( position );

        if( implementSegment.source() != point2D ) {
          implementSegment = Segment_2( point2D, implementSegment.target() );
          createPlanAB();
        }
      }
    }

    void setPoseRightEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = toPoint3( position );

        auto point2D = to2D( position );

        if( implementSegment.target() != point2D ) {
          implementSegment = Segment_2( implementSegment.source(), point2D );
          createPlanAB();
        }
      }
    }

    void setField( std::shared_ptr<Polygon_with_holes_2> field ) {
      currentField = field;
    }

    void setAPoint() {
      aPointTransform->setTranslation( toQVector3D( position ) );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );
      pointsEntity->setEnabled( false );

      aPoint = position;
      abPolyline.clear();
      abPolyline.push_back( to2D( position ) );
    }

    void setBPoint() {
      bPointTransform->setTranslation( toQVector3D( position ) );
      bPointEntity->setEnabled( true );

      bPoint = position;
      abPolyline.push_back( to2D( position ) );

      abSegment = Segment_3( aPoint, bPoint );

      createPlanAB();
    }

    void setAdditionalPoint() {
      abPolyline.push_back( to2D( position ) );
      createPlanAB();
    }

    void setAdditionalPointsContinous( const bool enabled ) {
      if( recordContinous && !enabled ) {
        Q_EMIT requestPolylineSimplification( &abPolyline, maxDeviation );
      }

      recordContinous = enabled;
    }

    void snap() {
      snapPlanAB();
    }

    void openAbLine();
    void openAbLineFromFile( QFile& file );

    void newField() {
      widget->resetToolbar();
      abPolyline.clear();
    }

    void saveAbLine();
    void saveAbLineToFile( QFile& file );

    void setPlannerSettings( const int pathsInReserve, const double maxDeviation ) {
      plan.pathsInReserve = pathsInReserve;
      this->maxDeviation = maxDeviation;

      if( abPolyline.size() > 2 ) {
        Q_EMIT requestPolylineSimplification( &abPolyline, maxDeviation );
      }
    }

    void setPassSettings( const int forwardPasses,
                          const int reversePasses,
                          const bool startRight,
                          const bool mirror ) {
      if( ( forwardPasses == 0 || reversePasses == 0 ) ) {
        this->forwardPasses = 0;
        this->reversePasses = 0;
      } else {
        this->forwardPasses = forwardPasses;
        this->reversePasses = reversePasses;
      }

      this->startRight = startRight;
      this->mirror = mirror;
    }

    void setPassNumberTo( const int /*passNumber*/ ) {}

    void setRunNumber( const uint32_t runNumber ) {
      this->runNumber = runNumber;
    }

    void createPlanPolyline( std::vector<Point_2>* polylinePtr );

  Q_SIGNALS:
    void planChanged( const Plan& );
    void requestPolylineSimplification( std::vector<Point_2>*, const double );

  private:
    void createPlanAB();
    void snapPlanAB();

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();

    int forwardPasses = 0;
    int reversePasses = 0;
    bool startRight = false;
    bool mirror = false;

    double maxDeviation = 0.1;

    std::shared_ptr<Polygon_with_holes_2> currentField;

    Point_3 aPoint = Point_3( 0, 0, 0 );
    Point_3 bPoint = Point_3( 0, 0, 0 );
    Segment_3 abSegment = Segment_3( Point_3( 0, 0, 0 ), Point_3( 0, 0, 0 ) );
    std::vector<Point_2> abPolyline;
    bool recordContinous = false;

    Segment_2 implementSegment = Segment_2( Point_2( 0, 0 ), Point_2( 0, 0 ) );

    Point_3 positionLeftEdgeOfImplement = Point_3( 0, 0, 0 );
    Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

    PlanGlobal plan = PlanGlobal();

    GlobalPlannerToolbar* widget = nullptr;
    KDDockWidgets::DockWidget* dock = nullptr;

    GeographicConvertionWrapper* tmw = nullptr;

  private:
    CgalThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;
    uint32_t runNumber = 0;

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;

    // markers
    Qt3DCore::QEntity* aPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* aPointMesh = nullptr;
    Qt3DCore::QTransform* aPointTransform = nullptr;
    Qt3DCore::QEntity* aTextEntity = nullptr;
    Qt3DCore::QTransform* aTextTransform = nullptr;

    Qt3DCore::QEntity* bPointEntity = nullptr;
    Qt3DExtras::QSphereMesh* bPointMesh = nullptr;
    Qt3DCore::QTransform* bPointTransform = nullptr;
    Qt3DCore::QEntity* bTextEntity = nullptr;
    Qt3DCore::QTransform* bTextTransform = nullptr;

    Qt3DCore::QEntity* pointsEntity = nullptr;
    Qt3DExtras::QSphereMesh* pointsMesh = nullptr;
    Qt3DExtras::QPhongMaterial* pointsMaterial = nullptr;

  private:
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;
};

class GlobalPlannerFactory : public BlockFactory {
    Q_OBJECT

  public:
    GlobalPlannerFactory( MyMainWindow* mainWindow,
                          KDDockWidgets::Location location,
                          QMenu* menu,
                          GeographicConvertionWrapper* tmw,
                          Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        mainWindow( mainWindow ),
        location( location ),
        menu( menu ),
        rootEntity( rootEntity ),
        tmw( tmw ) {
    }

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner Lines" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Guidance" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* object = new GlobalPlanner( getNameOfFactory() + QString::number( id ),
                                        mainWindow,
                                        tmw,
                                        rootEntity );
      auto* b = createBaseBlock( scene, object, id, true );

      object->dock->setTitle( QStringLiteral( "Global Planner" ) );
      object->dock->setWidget( object->widget );

      menu->addAction( object->dock->toggleAction() );

      mainWindow->addDockWidget( object->dock, location );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      b->addInputPort( QStringLiteral( "Field" ), QLatin1String( SLOT( setField( std::shared_ptr<Polygon_with_holes_2> ) ) ) );

      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

      return b;
    }

  private:
    MyMainWindow* mainWindow = nullptr;
    KDDockWidgets::Location location;
    QMenu* menu = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;
};
