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

#include "FieldManager.h"

#include "ThreadWeaver/DebuggingAids"

#include "qneblock.h"
#include "qneport.h"

#include <QScopedPointer>

#include <QFile>
#include <QFileDialog>
#include <QVector>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

#include "3d/BufferMesh.h"

#include "gui/FieldsOptimitionToolbar.h"

#include "kinematic/PathPrimitive.h"

#include "helpers/cgalHelper.h"

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

#include "helpers/GeoJsonHelper.h"

FieldManager::FieldManager( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
    : mainWindow( mainWindow ), tmw( tmw ) {
  ThreadWeaver::setDebugLevel( true, 0 );

  // test for recording
  {
    m_baseEntity    = new Qt3DCore::QEntity( rootEntity );
    m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
    m_baseEntity->addComponent( m_baseTransform );

    m_pointsEntity    = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity  = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity->setEnabled( false );
    m_segmentsEntity2->setEnabled( false );
    m_segmentsEntity3->setEnabled( false );

    m_pointsMesh = new BufferMesh( m_pointsEntity );
    m_pointsMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Points );
    m_pointsEntity->addComponent( m_pointsMesh );

    m_segmentsMesh = new BufferMesh( m_segmentsEntity );
    m_segmentsMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::LineStrip );
    m_segmentsEntity->addComponent( m_segmentsMesh );

    m_segmentsMesh2 = new BufferMesh( m_segmentsEntity2 );
    m_segmentsMesh2->view()->setPrimitiveType( Qt3DCore::QGeometryView::LineStrip );
    m_segmentsEntity2->addComponent( m_segmentsMesh2 );

    m_segmentsMesh3 = new BufferMesh( m_segmentsEntity3 );
    m_segmentsMesh3->view()->setPrimitiveType( Qt3DCore::QGeometryView::Points );
    m_segmentsEntity3->addComponent( m_segmentsMesh3 );

    m_segmentsMesh4 = new BufferMesh( m_segmentsEntity4 );
    m_segmentsMesh4->view()->setPrimitiveType( Qt3DCore::QGeometryView::LineStrip );
    m_segmentsEntity4->addComponent( m_segmentsMesh4 );

    m_pointsMaterial    = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
    m_segmentsMaterial  = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
    m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
    m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
    m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

    m_pointsMaterial->setAmbient( Qt::yellow );
    m_segmentsMaterial->setAmbient( Qt::white );
    m_segmentsMaterial2->setAmbient( Qt::green );
    m_segmentsMaterial3->setAmbient( Qt::blue );
    m_segmentsMaterial4->setAmbient( Qt::red );

    m_pointsEntity->addComponent( m_pointsMaterial );
    m_segmentsEntity->addComponent( m_segmentsMaterial );
    m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
    m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
    m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
  }

  // FIXME remove!
  auto file = QFile( "/home/christian/Documents/PlatformIO/Projects/QtOpenGuidance/fields/test-8.geojson" );
  openFieldFromFile( file );
}

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
          points.push_back( positionLeftEdgeOfImplement );
          recordNextPoint = false;
          recalculateField();
        } else {
          if( recordContinous ) {
            points.push_back( positionLeftEdgeOfImplement );
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
          points.push_back( positionRightEdgeOfImplement );
          recordNextPoint = false;
          recalculateField();
        } else {
          if( recordContinous ) {
            points.push_back( positionRightEdgeOfImplement );
            recordNextPoint = false;
            Q_EMIT pointsRecordedChanged( points.size(), CalculationOption::Option::None );
          }
        }
      }
    }
  }
}

void
FieldManager::openField() {
  std::cout << "FieldManager::openField" << std::endl;
  qDebug() << "FieldManager::openField";

  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow, tr( "Open Saved Field" ), dir, selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;GeoJSON Files (*.geojson)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect
  // to the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( QUrl::fromPercentEncoding( fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        openFieldFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#else
  QObject::connect( fileDialog, &QFileDialog::fileSelected, mainWindow, [this, fileDialog]( const QString& fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {
        openFieldFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#endif

  // connect finished to deleteLater, so the dialog gets deleted when Cancel is pressed
  QObject::connect( fileDialog, &QFileDialog::finished, fileDialog, &QFileDialog::deleteLater );

  fileDialog->open();
}

void
FieldManager::openFieldFromFile( QFile& file ) {
  if( !file.isOpen() ) {
    if( !file.open( QIODevice::ReadOnly ) ) {
      qWarning() << "Couldn't open save file.";
      return;
    }
  }

  std::cout << "FieldManager::openFieldFromFile" << file.fileName().toStdString() << std::endl;
  qDebug() << "FieldManager::openFieldFromFile " << file.fileName();

  auto geoJsonHelper = GeoJsonHelper( file );

  auto newField     = bool( false );
  auto newRawPoints = bool( false );

  for( const auto& member : geoJsonHelper.members ) {
    switch( member.first ) {
      case GeoJsonHelper::GeometryType::Polygon: {
        QVector< QVector3D > positions;
        Polygon_2            poly;

        const auto& polygon = std::get< GeoJsonHelper::PolygonType >( member.second );

        if( !polygon.empty() ) {
          for( const auto& point : polygon.front() ) {
            auto tmwPoint = tmw->Forward( point );
            positions.push_back( toQVector3D( tmwPoint ) );
            poly.push_back( toPoint2( tmwPoint ) );
          }

          currentField = std::make_shared< Polygon_with_holes_2 >( poly );

          const auto& outerPoly = currentField->outer_boundary();
          Q_EMIT pointsInFieldBoundaryChanged( outerPoly.size(), CalculationOption::Option::None );

          newField = true;

          m_segmentsMesh2->bufferUpdate( positions );
          m_segmentsEntity2->setEnabled( true );

          Q_EMIT fieldChanged( currentField );
        }
      } break;

      case GeoJsonHelper::GeometryType::MultiPoint: {
        QVector< QVector3D > positions;
        points.clear();

        for( const auto& point : std::get< GeoJsonHelper::MultiPointType >( member.second ) ) {
          auto tmwPoint = tmw->Forward( point );
          positions.push_back( toQVector3D( tmwPoint ) );
          points.emplace_back( toPoint3( tmwPoint ) );
        }

        m_segmentsMesh3->bufferUpdate( positions );
        m_segmentsEntity3->setEnabled( true );

        Q_EMIT pointsGeneratedForFieldBoundaryChanged( 0, CalculationOption::Option::None );
        Q_EMIT pointsRecordedChanged( points.size(), CalculationOption::Option::None );

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
}

void
FieldManager::saveField() {
  if( currentField || !points.empty() ) {
    QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
    QString dir;
    QString fileName = QFileDialog::getSaveFileName(
      mainWindow, tr( "Save Field" ), dir, tr( "All Files (*);;GeoJSON Files (*.geojson)" ), &selectedFilter );

    if( !fileName.isEmpty() ) {
      QFile saveFile( fileName );

      if( !saveFile.open( QIODevice::WriteOnly ) ) {
        qWarning() << "Couldn't open save file.";
        return;
      }

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

    auto pointsOfPolygon = GeoJsonHelper::MultiPointType();

    for( const auto& vi : currentField->outer_boundary() ) {
      pointsOfPolygon.emplace_back( tmw->Reverse( toEigenVector( vi ) ) );
    }

    // add the first point again to close the polygon
    pointsOfPolygon.push_back( pointsOfPolygon.front() );

    polygon.emplace_back( pointsOfPolygon );
    geoJsonHelper.addFeature( GeoJsonHelper::GeometryType::Polygon, polygon );
  }

  // the raw points
  {
    auto rawPoints = GeoJsonHelper::MultiPointType();

    for( const auto& point : points ) {
      rawPoints.emplace_back( tmw->Reverse( toEigenVector( point ) ) );
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

  QVector< QVector3D > meshSegmentPoints;

  for( const auto& vi : field->outer_boundary() ) {
    meshSegmentPoints << toQVector3D( vi, 0.1f );
  }

  meshSegmentPoints << meshSegmentPoints.first();
  m_segmentsMesh4->bufferUpdate( meshSegmentPoints );

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
  auto* object = new FieldManager( mainWindow, rootEntity, tmw );
  auto* b      = createBaseBlock( scene, object, id, true );
  object->moveToThread( thread );
  addCompressedObject( object );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Field" ), QLatin1String( SIGNAL( fieldChanged( std::shared_ptr< Polygon_with_holes_2 > ) ) ) );

  b->addOutputPort( QStringLiteral( "Points Recorded" ), QLatin1String( SIGNAL( pointsRecordedChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Points Generated" ),
                    QLatin1String( SIGNAL( pointsGeneratedForFieldBoundaryChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Points Boundary" ), QLatin1String( SIGNAL( pointsInFieldBoundaryChanged( NUMBER_SIGNATURE ) ) ) );

  addCompressedSignal( QMetaMethod::fromSignal( &FieldManager::setPose ) );
  addCompressedSignal( QMetaMethod::fromSignal( &FieldManager::setPoseLeftEdge ) );
  addCompressedSignal( QMetaMethod::fromSignal( &FieldManager::setPoseRightEdge ) );

  return b;
}
