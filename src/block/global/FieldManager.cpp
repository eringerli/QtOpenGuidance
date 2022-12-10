// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FieldManager.h"

#include "block/graphical/FieldModel.h"

#include "ThreadWeaver/DebuggingAids"

#include "qneblock.h"
#include "qneport.h"

#include <QScopedPointer>

#include <QFile>
#include <QFileDialog>
#include <QVector>

#include "gui/FieldsOptimitionToolbar.h"

#include "gui/OpenSaveHelper.h"

#include "kinematic/PathPrimitive.h"

#include "helpers/cgalHelper.h"

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

#include "helpers/GeoJsonHelper.h"

FieldManager::FieldManager( QWidget* mainWindow, GeographicConvertionWrapper* tmw ) : tmw( tmw ) {
  ThreadWeaver::setDebugLevel( true, 0 );

  openSaveHelper = new OpenSaveHelper( "Open Field", "GeoJSON Files (*.geojson)", mainWindow );

  QObject::connect( openSaveHelper, &OpenSaveHelper::openFile, this, &FieldManager::openFieldFromString );
  QObject::connect( openSaveHelper, &OpenSaveHelper::saveFile, this, &FieldManager::saveFieldToString );

  // FIXME remove!
  auto file = QFile( "/home/christian/Schreibtisch/QtOpenGuidance/fields/test-8.geojson" );
  openFieldFromFile( file );
}

FieldManager::~FieldManager() {}

void
FieldManager::alphaShape() {
  QElapsedTimer timer;
  timer.start();

  // you need at least 3 points for an area
  if( points.size() >= 3 ) {
    // make a 2D copy of the recorded points
    auto pointsCopy2D = std::make_shared< std::vector< Epick::Point_2 > >();
    pointsCopy2D->reserve( points.size() );

    for( const auto& point : points ) {
      pointsCopy2D->emplace_back( point.x(), point.y() );
    }

    // deletes the old one automatically and therefore stops all the old jobs
    if( fieldOptimitionController ) {
      fieldOptimitionController->stop();
      //      fieldOptimitionController->deleteLater();
    }

    //    fieldOptimitionController.clear();
    fieldOptimitionController = new FieldOptimitionController( pointsCopy2D );

    QObject::connect( fieldOptimitionController, &FieldOptimitionController::alphaChanged, this, &FieldManager::alphaChanged );
    QObject::connect(
      fieldOptimitionController, &FieldOptimitionController::fieldStatisticsChanged, this, &FieldManager::fieldStatisticsChanged );
    QObject::connect(
      fieldOptimitionController, &FieldOptimitionController::fieldOptimitionFinished, this, &FieldManager::alphaShapeFinished );

    fieldOptimitionController->run( alphaType, customAlpha, maxDeviation, distanceBetweenConnectPoints );
  }
}

void
FieldManager::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    this->position    = toPoint3( position );
    this->orientation = orientation;
  }
}

void
FieldManager::setPoseLeftEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    if( options.testFlag( CalculationOption::CalculateLocalOffsets ) && options.testFlag( CalculationOption::NoOrientation ) ) {
      positionLeftEdgeOfImplement = toPoint3( position );
    } else {
      if( !recordOnRightEdgeOfImplement ) {
        if( recordNextPoint ) {
          points.push_back( toPoint3( position ) );
          recordNextPoint = false;
          recalculateField();
        } else {
          if( recordContinous ) {
            points.push_back( toPoint3( position ) );
            Q_EMIT pointAdded( position );
            recordNextPoint = false;
            Q_EMIT pointsRecordedChanged( points.size(), CalculationOption::Option::None );
          }
        }
      }
    }
  }
}

void
FieldManager::setPoseRightEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    if( options.testFlag( CalculationOption::CalculateLocalOffsets ) && options.testFlag( CalculationOption::NoOrientation ) ) {
      positionRightEdgeOfImplement = toPoint3( position );
      ;
    } else {
      if( recordOnRightEdgeOfImplement ) {
        if( recordNextPoint ) {
          points.push_back( toPoint3( position ) );
          recordNextPoint = false;
          recalculateField();
        } else {
          if( recordContinous ) {
            points.push_back( toPoint3( position ) );
            Q_EMIT pointAdded( position );
            recordNextPoint = false;
            Q_EMIT pointsRecordedChanged( points.size(), CalculationOption::Option::None );
          }
        }
      }
    }
  }
}

void
FieldManager::openFieldFromString( const QString& fileName ) {
  if( !fileName.isEmpty() ) {
    // some string wrangling on android to get the native file name
    QFile loadFile( fileName );

    if( !loadFile.open( QIODevice::ReadOnly ) ) {
      qWarning() << "Couldn't open save file.";
    } else {
      openFieldFromFile( loadFile );
    }
  }
}

void
FieldManager::openFieldFromFile( QFile& file ) {
  if( !file.isOpen() ) {
    if( !file.open( QIODevice::ReadOnly ) ) {
      qWarning() << "Couldn't open save file.";
      return;
    }
  }

  auto geoJsonHelper = GeoJsonHelper( file );

  auto newField     = bool( false );
  auto newRawPoints = bool( false );

  for( const auto& member : geoJsonHelper.members ) {
    switch( member.first ) {
      case GeoJsonHelper::GeometryType::Polygon: {
        Polygon_2 poly;

        const auto& polygon = std::get< GeoJsonHelper::PolygonType >( member.second );

        if( !polygon.empty() ) {
          for( const auto& point : polygon.front() ) {
            auto tmwPoint = tmw->Forward( point );
            poly.push_back( toPoint2( tmwPoint ) );
          }

          currentField = std::make_shared< Polygon_with_holes_2 >( poly );

          const auto& outerPoly = currentField->outer_boundary();
          Q_EMIT pointsInFieldBoundaryChanged( outerPoly.size(), CalculationOption::Option::None );

          newField = true;

          Q_EMIT fieldChanged( currentField );
        }
      } break;

      case GeoJsonHelper::GeometryType::MultiPoint: {
        points.clear();

        const auto& multiPoint = std::get< GeoJsonHelper::MultiPointType >( member.second );

        for( const auto& point : multiPoint ) {
          auto tmwPoint = tmw->Forward( point );
          points.push_back( toPoint3( tmwPoint ) );
        }

        Q_EMIT pointsGeneratedForFieldBoundaryChanged( 0, CalculationOption::Option::None );
        Q_EMIT pointsRecordedChanged( points.size(), CalculationOption::Option::None );

        Q_EMIT pointsSet( points );

        newRawPoints = true;
      } break;

      default:
        break;
    }
  }

  // if a file is loaded with only raw points, then recalculate() to set it as new
  // boundary
  if( newRawPoints && !newField ) {
    recalculateField();
  }
}

void
FieldManager::newField() {
  points.clear();
  Q_EMIT fieldCleared();
}

void
FieldManager::saveFieldToString( QString fileName ) {
  if( !fileName.isEmpty() ) {
    // some string wrangling on android to get the native file name
    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning() << "Couldn't open save file.";
    } else {
      saveFieldToFile( saveFile );
    }
  }
}

void
FieldManager::saveFieldToFile( QFile& file ) {
  auto geoJsonHelper = GeoJsonHelper();

  // the refined polygon
  {
    auto polygon = GeoJsonHelper::PolygonType();

    auto pointsOfPolygon = GeoJsonHelper::PointVector();

    for( const auto& vi : currentField->outer_boundary() ) {
      pointsOfPolygon.push_back( tmw->Reverse( toEigenVector( vi ) ) );
    }

    // add the first point again to close the polygon

    auto polygonPoints = GeoJsonHelper::PointVector();

    for( const auto& point : pointsOfPolygon ) {
      polygonPoints.push_back( removeZ( point ) );
    }

    polygon.push_back( std::move( polygonPoints ) );

    geoJsonHelper.addFeature( GeoJsonHelper::GeometryType::Polygon, polygon );
  }

  // the raw points
  {
    auto rawPoints = GeoJsonHelper::MultiPointType();

    for( const auto& point : points ) {
      rawPoints.push_back( tmw->Reverse( toEigenVector( point ) ) );
    }

    geoJsonHelper.addFeature( GeoJsonHelper::GeometryType::MultiPoint, rawPoints );
  }

  geoJsonHelper.save( file );
}

void
FieldManager::setContinousRecord( const bool enabled ) {
  if( recordContinous == true && enabled == false ) {
    recalculateField();
  }

  recordContinous = enabled;
}

void
FieldManager::recordPoint() {
  recordNextPoint = true;
}

void
FieldManager::recordOnEdgeOfImplementChanged( const bool right ) {
  recordOnRightEdgeOfImplement = right;
}

void
FieldManager::recalculateField() {
  Q_EMIT pointsSet( points );

  alphaShape();
}

void
FieldManager::setRecalculateFieldSettings( const FieldsOptimitionToolbar::AlphaType alphaType,
                                           const double                             customAlpha,
                                           const double                             maxDeviation,
                                           const double                             distanceBetweenConnectPoints ) {
  this->alphaType                    = alphaType;
  this->customAlpha                  = customAlpha;
  this->maxDeviation                 = maxDeviation;
  this->distanceBetweenConnectPoints = distanceBetweenConnectPoints;
}

void
FieldManager::alphaShapeFinished( const std::shared_ptr< Polygon_with_holes_2 >& field, const double /*alpha*/ ) {
  currentField = field;

  Q_EMIT fieldChanged( currentField );
}

void
FieldManager::fieldStatisticsChanged( const double pointsRecorded,
                                      const double pointsGeneratedForFieldBoundary,
                                      const double pointsInFieldBoundary ) {
  std::cout << "FieldManager::fieldStatisticsChanged " << this << ", " << pointsRecorded << ", " << pointsGeneratedForFieldBoundary << ", "
            << pointsInFieldBoundary << std::endl;

  Q_EMIT pointsRecordedChanged( pointsRecorded, CalculationOption::Option::None );
  Q_EMIT pointsGeneratedForFieldBoundaryChanged( pointsGeneratedForFieldBoundary, CalculationOption::Option::None );
  Q_EMIT pointsInFieldBoundaryChanged( pointsInFieldBoundary, CalculationOption::Option::None );
}

QNEBlock*
FieldManagerFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FieldManager( mainWindow, tmw );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Field" ), QLatin1String( SIGNAL( fieldChanged( std::shared_ptr< Polygon_with_holes_2 > ) ) ) );

  b->addOutputPort( QStringLiteral( "Points Recorded" ), QLatin1String( SIGNAL( pointsRecordedChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Points Generated" ),
                    QLatin1String( SIGNAL( pointsGeneratedForFieldBoundaryChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Points Boundary" ), QLatin1String( SIGNAL( pointsInFieldBoundaryChanged( NUMBER_SIGNATURE ) ) ) );

  auto fieldModel = new FieldModel( rootEntity );
  b->addObject( fieldModel );

  QObject::connect( obj, &FieldManager::fieldChanged, fieldModel, &FieldModel::setField );
  QObject::connect( obj, &FieldManager::fieldCleared, fieldModel, &FieldModel::clearField );
  QObject::connect( obj, &FieldManager::pointAdded, fieldModel, &FieldModel::addPoint );
  QObject::connect( obj, &FieldManager::pointsSet, fieldModel, &FieldModel::setPoints );
  QObject::connect( obj, &FieldManager::pointsCleared, fieldModel, &FieldModel::clearPoints );

  return b;
}
