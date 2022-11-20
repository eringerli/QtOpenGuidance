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

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

#include "helpers/GeographicConvertionWrapper.h"

#include "gui/FieldsOptimitionToolbar.h"

#include "job/FieldOptimitionController.h"

class QFile;

#include <utility>

class CgalThread;
class CgalWorker;

class FieldModel;

class FieldManager : public BlockBase {
  Q_OBJECT

public:
  explicit FieldManager( QWidget* mainWindow, GeographicConvertionWrapper* tmw );

  ~FieldManager();

private:
  void alphaShape();

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );

  void setPoseLeftEdge( POSE_SIGNATURE_SLOT );
  void setPoseRightEdge( POSE_SIGNATURE_SLOT );

  void newField();

  void openField();
  void openFieldFromString( const QString& fileName );
  void openFieldFromFile( QFile& file );

  void saveField();
  void saveFieldToString( QString fileName );
  void saveFieldToFile( QFile& file );

  void setContinousRecord( const bool enabled );
  void recordPoint();

  void recordOnEdgeOfImplementChanged( const bool right );

  void recalculateField();

  void setRecalculateFieldSettings( const FieldsOptimitionToolbar::AlphaType alphaType,
                                    const double                             customAlpha,
                                    const double                             maxDeviation,
                                    const double                             distanceBetweenConnectPoints );

  void alphaShapeFinished( const std::shared_ptr< Polygon_with_holes_2 >& field, const double alpha );

  void
  fieldStatisticsChanged( const double pointsRecorded, const double pointsGeneratedForFieldBoundary, const double pointsInFieldBoundary );

Q_SIGNALS:
  void requestFieldOptimition( const uint32_t                           runNumber,
                               std::vector< Epick::Point_2 >*           points,
                               const FieldsOptimitionToolbar::AlphaType alphaType,
                               const double                             customAlpha,
                               const double                             maxDeviation,
                               const double                             distanceBetweenConnectPoints );

  void alphaChanged( const double optimal, const double solid );
  void fieldChanged( std::shared_ptr< Polygon_with_holes_2 > );

  void pointsRecordedChanged( NUMBER_SIGNATURE_SIGNAL );
  void pointsGeneratedForFieldBoundaryChanged( NUMBER_SIGNATURE_SIGNAL );
  void pointsInFieldBoundaryChanged( NUMBER_SIGNATURE_SIGNAL );

  void pointAdded( const Eigen::Vector3d& );
  void pointsSet( const std::vector< Epick::Point_3 >& );
  void pointsCleared();
  void fieldCleared();

public:
  Point_3            position    = Point_3( 0, 0, 0 );
  Eigen::Quaterniond orientation = Eigen::Quaterniond( 0, 0, 0, 0 );

  Point_3 positionLeftEdgeOfImplement  = Point_3( 0, 0, 0 );
  Point_3 positionRightEdgeOfImplement = Point_3( 0, 0, 0 );

private:
  QWidget*                     mainWindow = nullptr;
  GeographicConvertionWrapper* tmw        = nullptr;

  std::vector< Epick::Point_3 > points;
  bool                          recordContinous              = false;
  bool                          recordNextPoint              = false;
  bool                          recordOnRightEdgeOfImplement = false;

private:
  FieldsOptimitionToolbar::AlphaType alphaType                    = FieldsOptimitionToolbar::AlphaType::Optimal;
  double                             customAlpha                  = 10;
  double                             maxDeviation                 = 0.1;
  double                             distanceBetweenConnectPoints = 0.5;

  QPointer< FieldOptimitionController > fieldOptimitionController;

  std::shared_ptr< Polygon_with_holes_2 > currentField;
  double                                  currentAlpha = 0;
};

class FieldManagerFactory : public BlockFactory {
  Q_OBJECT

public:
  FieldManagerFactory( QThread* thread, QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
      : BlockFactory( thread ), mainWindow( mainWindow ), rootEntity( rootEntity ), tmw( tmw ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Field Manager" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Base Blocks" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  QWidget*                     mainWindow = nullptr;
  Qt3DCore::QEntity*           rootEntity = nullptr;
  GeographicConvertionWrapper* tmw        = nullptr;
};
