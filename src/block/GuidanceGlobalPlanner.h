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

#ifndef GLOBALPLANNER_H
#define GLOBALPLANNER_H

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

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include "../kinematic/CgalWorker.h"
#include "../kinematic/TransverseMercatorWrapper.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class GlobalPlanner : public BlockBase {
    Q_OBJECT

  public:
    explicit GlobalPlanner( Qt3DCore::QEntity* rootEntity, TransverseMercatorWrapper* tmw );

    ~GlobalPlanner(){
      cgalWorker->deleteLater();
      threadForCgalWorker->deleteLater();
    }

  private:
    void alphaShape();

  public slots:
    void setPose( const Point_3& position, const QQuaternion orientation, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation = orientation;

        aPointTransform->setRotation( orientation );
        bPointTransform->setRotation( orientation );
      }
    }

    void setPoseLeftEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionLeftEdgeOfImplement = position;

        Point_2 point2D( double( position.x() ), double( position.y() ) );

        if( implementLine.source() != point2D ) {
          implementLine = Segment_2( point2D, implementLine.target() );
//          createPlanAB();
        }
      } else {
        if( recordOnRightEdgeOfImplement == false ) {
          if( recordNextPoint ) {
            points.push_back( position );
            recordNextPoint = false;
            recalculateField();
          } else {
            if( recordContinous ) {
              points.push_back( position );
              recordNextPoint = false;
            }
          }
        }
      }
    }

    void setPoseRightEdge( const Point_3& position, const QQuaternion, const PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateLocalOffsets ) &&
          options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        positionRightEdgeOfImplement = position;

        Point_2 point2D( double( position.x() ), double( position.y() ) );

        if( implementLine.target() != point2D ) {
          implementLine = Segment_2( implementLine.source(), point2D );
//          createPlanAB();
        }
      } else {
        if( recordOnRightEdgeOfImplement == true ) {
          if( recordNextPoint ) {
            points.push_back( position );
            recordNextPoint = false;
            recalculateField();
          } else {
            if( recordContinous ) {
              points.push_back( position );
              recordNextPoint = false;
            }
          }
        }
      }
    }

    void openField() {}
    void newField() {
      points.clear();
    }
    void saveField() {}

    void setContinousRecord( bool enabled ) {
      if( recordContinous == true && enabled == false ) {
        recalculateField();
      }

      recordContinous = enabled;
    }
    void recordPoint() {
      recordNextPoint = true;
    }

    void recordOnEdgeOfImplementChanged( bool right ) {
      recordOnRightEdgeOfImplement = right;
    }

    void recalculateField() {
      alphaShape();
    }

    void setRecalculateFieldSettings( FieldsOptimitionToolbar::AlphaType alphaType, double customAlpha, double maxDeviation, double distanceBetweenConnectPoints ) {
      this->alphaType = alphaType;
      this->customAlpha = customAlpha;
      this->maxDeviation = maxDeviation;
      this->distanceBetweenConnectPoints = distanceBetweenConnectPoints;
      recalculateField();
    }

    void a_clicked() {
      aPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );

      aPointEntity->setEnabled( true );
      bPointEntity->setEnabled( false );

      aPoint = position;

      qDebug() << "a_clicked()"/* << aPoint*/;
    }

    void createPlanAB();

    void b_clicked() {
      bPointTransform->setTranslation( convertPoint3ToQVector3D( position ) );
      bPointEntity->setEnabled( true );

      bPoint = position;

      abLine = Segment_3( aPoint, bPoint );

      createPlanAB();

//      optimalTransportationReconstruction();
//      alphaShape();
    }

    void snap_clicked() {
//      optimalTransportationReconstruction();
      alphaShape();

      qDebug() << "snap_clicked()";
    }

    void turnLeft_clicked() {
      qDebug() << "turnLeft_clicked()";
    }
    void turnRight_clicked() {
      qDebug() << "turnRight_clicked()";
    }

    void setPlannerSettings( int pathsToGenerate, int pathsInReserve ) {
      this->pathsToGenerate = pathsToGenerate;
      this->pathsInReserve = pathsInReserve;

      createPlanAB();
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

      createPlanAB();
    }

    void setPassNumberTo( int /*passNumber*/ ) {}

    void alphaShapeFinished( Polygon_with_holes_2* );

  signals:
    void planChanged( QVector<QSharedPointer<PathPrimitive>> );

    void alphaChanged( double optimal, double solid );
    void fieldStatisticsChanged( double, double, double );
    void requestFieldOptimition( std::vector<K::Point_2>* points,
                                 FieldsOptimitionToolbar::AlphaType alphaType,
                                 double customAlpha,
                                 double maxDeviation,
                                 double distanceBetweenConnectPoints );

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();

    int pathsToGenerate = 5;
    int pathsInReserve = 3;
    int forwardPasses = 0;
    int reversePasses = 0;
    bool startRight = false;
    bool mirror = false;

    Point_3 aPoint = Point_3();
    Point_3 bPoint = Point_3();
    Segment_3 abLine = Segment_3();

    Segment_2 implementLine = Segment_2();

    Point_3 positionLeftEdgeOfImplement = Point_3();
    Point_3 positionRightEdgeOfImplement = Point_3();

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    TransverseMercatorWrapper* tmw = nullptr;

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

    std::vector<K::Point_3> points;
    bool recordContinous = false;
    bool recordNextPoint = false;
    bool recordOnRightEdgeOfImplement = false;

  private:
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    FieldsOptimitionToolbar::AlphaType alphaType = FieldsOptimitionToolbar::AlphaType::Optimal;
    double customAlpha = 10;
    double maxDeviation = 0.1;
    double distanceBetweenConnectPoints = 0.5;

    QThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;

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
    GlobalPlannerFactory( Qt3DCore::QEntity* rootEntity, TransverseMercatorWrapper* tmw )
      : BlockFactory(),
        rootEntity( rootEntity ),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Global Planner" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new GlobalPlanner( rootEntity, tmw );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "A clicked" ), QLatin1String( SLOT( a_clicked() ) ) );
      b->addInputPort( QStringLiteral( "B clicked" ), QLatin1String( SLOT( b_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Snap clicked" ), QLatin1String( SLOT( snap_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Turn Left" ), QLatin1String( SLOT( turnLeft_clicked() ) ) );
      b->addInputPort( QStringLiteral( "Turn Right" ), QLatin1String( SLOT( turnRight_clicked() ) ) );

      b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( QVector<QSharedPointer<PathPrimitive>> ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    TransverseMercatorWrapper* tmw = nullptr;
};

#endif // GLOBALPLANNER_H

