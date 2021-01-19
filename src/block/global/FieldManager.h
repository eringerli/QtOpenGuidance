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

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/cgalHelper.h"
#include "kinematic/PoseOptions.h"

#include "helpers/GeographicConvertionWrapper.h"

#include "gui/FieldsOptimitionToolbar.h"

class QFile;

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

  public Q_SLOTS:
    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options );

    void setPoseLeftEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options );

    void setPoseRightEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options );

    void openField();
    void openFieldFromFile( QFile& file );

    void newField() {
      points.clear();
    }

    void saveField();
    void saveFieldToFile( QFile& file );

    void setContinousRecord( const bool enabled );
    void recordPoint();

    void recordOnEdgeOfImplementChanged( const bool right );

    void recalculateField();

    void setRecalculateFieldSettings( const FieldsOptimitionToolbar::AlphaType alphaType,
                                      const double customAlpha,
                                      const double maxDeviation,
                                      const double distanceBetweenConnectPoints );

    void setRunNumber( const uint32_t runNumber );

    void alphaShapeFinished( const std::shared_ptr<Polygon_with_holes_2>& field, const double alpha );

    void fieldStatisticsChanged( const double pointsRecorded,
                                 const double pointsGeneratedForFieldBoundary,
                                 const double pointsInFieldBoundary );

  Q_SIGNALS:
    void fieldChanged( std::shared_ptr<Polygon_with_holes_2> );

    void alphaChanged( const double optimal, const double solid );
    void requestFieldOptimition( const uint32_t runNumber,
                                 std::vector<Epick::Point_2>* points,
                                 const FieldsOptimitionToolbar::AlphaType alphaType,
                                 const double customAlpha,
                                 const double maxDeviation,
                                 const double distanceBetweenConnectPoints );
    void requestNewRunNumber();

    void pointsRecordedChanged( const double );
    void pointsGeneratedForFieldBoundaryChanged( const double );
    void pointsInFieldBoundaryChanged( const double );

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
    FieldsOptimitionToolbar::AlphaType alphaType = FieldsOptimitionToolbar::AlphaType::Optimal;
    double customAlpha = 10;
    double maxDeviation = 0.1;
    double distanceBetweenConnectPoints = 0.5;

    CgalThread* threadForCgalWorker = nullptr;
    CgalWorker* cgalWorker = nullptr;
    uint32_t runNumber = 0;

    std::shared_ptr<Polygon_with_holes_2> currentField;
    double currentAlpha = 0;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;
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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    QWidget* mainWindow = nullptr;
    Qt3DCore::QEntity* rootEntity = nullptr;
    GeographicConvertionWrapper* tmw = nullptr;
};
