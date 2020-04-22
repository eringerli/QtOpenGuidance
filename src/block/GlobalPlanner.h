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

#include <QQuaternion>
#include <QVector3D>
#include <QPointF>
#include <QPolygonF>
#include <QLineF>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../3d/BufferMesh.h"

#include "../gui/FieldsOptimitionToolbar.h"
#include "../gui/GlobalPlannerToolbar.h"

#include "../gui/MyMainWindow.h"
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

#include "../kinematic/PathPrimitive.h"
#include "../kinematic/PathPrimitiveLine.h"
#include "../kinematic/PathPrimitiveRay.h"
#include "../kinematic/PathPrimitiveSegment.h"
#include "../kinematic/PathPrimitiveSequence.h"

#include "../kinematic/PlanGlobal.h"

#include "../kinematic/GeographicConvertionWrapper.h"

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

  public slots:
    void setPose( const Point_3& position, const QQuaternion orientation, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );

//        QElapsedTimer timer;
//        timer.start();
        plan.expand( to2D( position ) );
//        qDebug() << "Cycle Time plan.expandPlan:" << timer.nsecsElapsed() << "ns";

        showPlan();
      }
    }

    void setPoseLeftEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;

        auto point2D = to2D( position );

        if( implementSegment.source() != point2D ) {
          implementSegment = Segment_2( point2D, implementSegment.target() );
          createPlanAB();
        }
      }
    }

    void setPoseRightEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;

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
      aPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );

      aPoint = position;
      abPolyline.clear();
      abPolyline.push_back( to2D( position ) );

      qDebug() << "a_clicked()"/* << aPoint*/;
    }

    void setBPoint() {
      qDebug() << "setBPoint()";
      bPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );
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

    void setAdditionalPointsContinous( bool /*enabled*/ ) {

    }

    void snap() {
      snapPlanAB();
      qDebug() << "snap()";
    }

    void openAbLine();
    void openAbLineFromFile( QFile& file );

    void newField() {
      widget->resetToolbar();
    }

    void saveAbLine();
    void saveAbLineToFile( QFile& file );

    void setPlannerSettings( int pathsInReserve ) {
      plan.pathsInReserve = pathsInReserve;

//      createPlanAB();
    }

    void setPassSettings( int forwardPasses, int reversePasses, bool startRight, bool mirror ) {
      if( ( forwardPasses == 0 || reversePasses == 0 ) ) {
        this->forwardPasses = 0;
        this->reversePasses = 0;
      } else {
        this->forwardPasses = forwardPasses;
        this->reversePasses = reversePasses;
      }

      this->startRight = startRight;
      this->mirror = mirror;

//      createPlanAB();
    }

    void setPassNumberTo( int /*passNumber*/ ) {}

    void setRunNumber( uint32_t runNumber ) {
      this->runNumber = runNumber;
    }



  signals:
    void planChanged( const Plan& );

//    void alphaChanged( double optimal, double solid );
//    void fieldStatisticsChanged( double, double, double );
//    void requestFieldOptimition( uint32_t runNumber,
//                                 std::vector<K::Point_2>* points,
//                                 FieldsOptimitionToolbar::AlphaType alphaType,
//                                 double customAlpha,
//                                 double maxDeviation,
//                                 double distanceBetweenConnectPoints );
//    void requestNewRunNumber();

  private:
    void createPlanAB();
    void snapPlanAB();
    void showPlan();

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();

    int forwardPasses = 0;
    int reversePasses = 0;
    bool startRight = false;
    bool mirror = false;

    std::shared_ptr<Polygon_with_holes_2> currentField;

    Point_3 aPoint = Point_3( 0, 0, 0 );
    Point_3 bPoint = Point_3( 0, 0, 0 );
    Segment_3 abSegment = Segment_3( Point_3( 0, 0, 0 ), Point_3( 0, 0, 0 ) );
    std::vector<Point_2> abPolyline;

    Segment_2 implementSegment = Segment_2( Point_2( 0, 0 ), Point_2( 0, 0 ) );

    Point_3 positionLeftEdgeOfImplement = Point_3( 0, 0, 0 );
    Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

    PlanGlobal plan = PlanGlobal();

    GlobalPlannerToolbar* widget = nullptr;
    KDDockWidgets::DockWidget* dock = nullptr;

    GeographicConvertionWrapper* tmw = nullptr;

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

  private:
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    CgalThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;
    uint32_t runNumber = 0;

    Qt3DCore::QEntity* m_pointsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity2 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity3 = nullptr;
    Qt3DCore::QEntity* m_segmentsEntity4 = nullptr;
    BufferMesh* m_pointsMesh = nullptr;
    BufferMesh* m_segmentsMesh = nullptr;
    BufferMesh* m_segmentsMesh2 = nullptr;
    BufferMesh* m_segmentsMesh3 = nullptr;
    BufferMesh* m_segmentsMesh4 = nullptr;
    Qt3DExtras::QPhongMaterial* m_pointsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial2 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial3 = nullptr;
    Qt3DExtras::QPhongMaterial* m_segmentsMaterial4 = nullptr;
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

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );

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
