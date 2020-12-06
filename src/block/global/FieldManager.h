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

#include "../BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../3d/BufferMesh.h"

#include "../gui/FieldsOptimitionToolbar.h"

#include "../helpers/cgalHelper.h"
#include "../helpers/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../kinematic/PathPrimitive.h"

#include "../helpers/GeographicConvertionWrapper.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class CgalThread;
class CgalWorker;

class FieldManager : public BlockBase {
    Q_OBJECT

  public:
    explicit FieldManager( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw );

    ~FieldManager() {}

  private:
    void alphaShape();

  public slots:
    void setPose( const Point_3 position, const Eigen::Quaterniond orientation, const PoseOption::Options options );

    void setPoseLeftEdge( const Point_3 position, const Eigen::Quaterniond, const PoseOption::Options options );

    void setPoseRightEdge( const Point_3 position, const Eigen::Quaterniond, const PoseOption::Options options );

    void openField();
    void openFieldFromFile( QFile& file );

    void newField() {
      points.clear();
    }

    void saveField();
    void saveFieldToFile( QFile& file );

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
    }

    void setRunNumber( uint32_t runNumber ) {
      this->runNumber = runNumber;
    }

    void alphaShapeFinished( std::shared_ptr<Polygon_with_holes_2> field, double alpha );

    void fieldStatisticsChanged( double pointsRecorded, double pointsGeneratedForFieldBoundary, double pointsInFieldBoundary ) {
      emit pointsRecordedChanged( pointsRecorded );
      emit pointsGeneratedForFieldBoundaryChanged( pointsGeneratedForFieldBoundary );
      emit pointsInFieldBoundaryChanged( pointsInFieldBoundary );
    }

  signals:
    void fieldChanged( std::shared_ptr<Polygon_with_holes_2> );

    void alphaChanged( double optimal, double solid );
    void requestFieldOptimition( uint32_t runNumber,
                                 std::vector<Epick::Point_2>* points,
                                 FieldsOptimitionToolbar::AlphaType alphaType,
                                 double customAlpha,
                                 double maxDeviation,
                                 double distanceBetweenConnectPoints );
    void requestNewRunNumber();

    void pointsRecordedChanged( double );
    void pointsGeneratedForFieldBoundaryChanged( double );
    void pointsInFieldBoundaryChanged( double );

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();

    Point_3 positionLeftEdgeOfImplement = Point_3( 0, 0, 0 );
    Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;

    std::vector<Epick::Point_3> points;
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

    CgalThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;
    uint32_t runNumber = 0;

    std::shared_ptr<Polygon_with_holes_2> currentField;
    double currentAlpha = 0;

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

class FieldManagerFactory : public BlockFactory {
    Q_OBJECT

  public:
    FieldManagerFactory( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
      : BlockFactory(),
        mainWindow( mainWindow ),
        rootEntity( rootEntity ),
        tmw( tmw ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Field Manager" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Base Blocks" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new FieldManager( mainWindow, rootEntity, tmw );
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->addOutputPort( QStringLiteral( "Field" ), QLatin1String( SIGNAL( fieldChanged( std::shared_ptr<Polygon_with_holes_2> ) ) ) );

      b->addOutputPort( QStringLiteral( "Points Recorded" ), QLatin1String( SIGNAL( pointsRecordedChanged( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Points Generated" ), QLatin1String( SIGNAL( pointsGeneratedForFieldBoundaryChanged( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Points Boundary" ), QLatin1String( SIGNAL( pointsInFieldBoundaryChanged( double ) ) ) );

      return b;
    }

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;
};
