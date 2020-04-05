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

#include "GlobalPlannerLines.h"
#include <QScopedPointer>

#include <QFileDialog>

#include "../cgal.h"
#include "../kinematic/CgalWorker.h"

bool GlobalPlannerLines::isLineAlreadyInPlan( const std::shared_ptr<PathPrimitiveLine>& line ) {
  for( const auto& step : * ( plan.plan ) ) {
    if( const auto* pathLine = step->castToLine() ) {
      if( ( *pathLine ) == ( *line ) ) {
        return true;
      }
    }
  }

  return false;
}

void GlobalPlannerLines::sortPlan() {
  sort( plan.plan->begin( ), plan.plan->end( ), [ ]( const auto & lhs, const auto & rhs ) {
    const auto* lhsLine = lhs->castToLine();
    const auto* rhsLine = rhs->castToLine();

    return lhsLine->line.has_on_positive_side( rhsLine->line.point( 0 ) );
  } );
}

void GlobalPlannerLines::clearPlan() {
  plan.plan->clear();
}

void GlobalPlannerLines::showPlan() {
  if( !plan.plan->empty() ) {
    const Point_2 position2D = to2D( position );

    constexpr double range = 25;
    Iso_rectangle_2 viewBox( Bbox_2( position2D.x() - range, position2D.y() - range, position2D.x() + range, position2D.y() + range ) );

    QVector<QVector3D> positions;

    for( const auto& step : * ( plan.plan ) ) {
      if( const auto* pathLine = step->castToLine() ) {
        const auto& line = pathLine->line;

        CGAL::cpp11::result_of<Intersect_2( Segment_2, Line_2 )>::type
        result = intersection( viewBox, line );

        if( result ) {
          if( const Segment_2* segment = boost::get<Segment_2>( &*result ) ) {
            positions << QVector3D( segment->source().x(), segment->source().y(), 0 );
            positions << QVector3D( segment->target().x(), segment->target().y(), 0 );
          }
        }
      }
    }

    m_segmentsMesh->bufferUpdate( positions );
    m_segmentsEntity->setEnabled( true );
  }
}

void GlobalPlannerLines::createPlanAB() {
  if( abSegment.squared_length() > 1 &&
      implementSegment.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    if( plan.plan->size() >= std::vector<std::shared_ptr<PathPrimitive>>::size_type( pathsInReserve * 2 + 1 ) ) {
      if( ( *( plan.plan->cbegin() + ( pathsInReserve - 1 ) ) )->castToLine()->line.has_on_positive_side( position2D ) &&
          ( *( plan.plan->cend() - pathsInReserve ) )->castToLine()->line.has_on_negative_side( position2D ) ) {
        return;
      }
    }

//    qDebug() << "Long way...";

    Segment_2 ab2dSegment = to2D( abSegment );
    Line_2 ab2D = ab2dSegment.supporting_line();
    Point_2 positionProjectedToAbLine = ab2D.projection( position2D );
    Segment_2 abPerpendicularSegment( position2D, positionProjectedToAbLine );

    double angleAbRad = angleOfLineRadians( ab2D );

    double implementWidth = std::sqrt( implementSegment.squared_length() );

    double distanceFromAbLine = std::sqrt( CGAL::squared_distance( position2D, positionProjectedToAbLine ) );

    if( ab2D.has_on_positive_side( position2D ) ) {
      distanceFromAbLine = -distanceFromAbLine;
    }

    int32_t passNumber = std::floor( distanceFromAbLine / implementWidth );

    double moduloDistanceFromAbLine = passNumber * implementWidth;

    for( int offsetCount = -pathsToGenerate ; offsetCount <= pathsToGenerate ; ++offsetCount ) {
      double offsetDistance = moduloDistanceFromAbLine + offsetCount * implementWidth;

      auto offsetVector = polarOffsetRad( angleAbRad + M_PI, offsetDistance );

      auto newLine = std::make_shared<PathPrimitiveLine>(
                       Line_2( ab2dSegment.source() - offsetVector, ab2dSegment.target() - offsetVector ),
                       implementWidth, true, passNumber + offsetCount );

      if( !isLineAlreadyInPlan( newLine ) ) {
        plan.plan->push_back( newLine );
      }
    }

    sortPlan();
    emit planChanged( plan );
  }
}

void GlobalPlannerLines::snapPlanAB() {
  if( abSegment.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    Segment_2 ab2dSegment = to2D( abSegment );
    Line_2 ab2D = ab2dSegment.supporting_line();
    Point_2 positionProjectedToAbLine = ab2D.projection( position2D );
    Segment_2 abPerpendicularSegment( position2D, positionProjectedToAbLine );

    double angleAbRad = angleOfLineRadians( ab2D );

    double implementWidth = std::sqrt( implementSegment.squared_length() );

    double distanceFromAbLine = std::sqrt( CGAL::squared_distance( position2D, positionProjectedToAbLine ) );

    if( ab2D.has_on_positive_side( position2D ) ) {
      distanceFromAbLine = -distanceFromAbLine;
    }

    int32_t passNumber = std::round( distanceFromAbLine / implementWidth );

    double moduloDistanceFromAbLine = passNumber * implementWidth;

    auto offsetVector = polarOffsetRad( M_PI + angleAbRad, distanceFromAbLine - moduloDistanceFromAbLine );
    auto offsetVector3D = to3D( offsetVector );

    abSegment = Segment_3( abSegment.source() - offsetVector3D, abSegment.target() - offsetVector3D );

    clearPlan();
    createPlanAB();
  }
}

GlobalPlannerLines::GlobalPlannerLines( const QString& uniqueName, MyMainWindow* mainWindow, GeographicConvertionWrapper* tmw, Qt3DCore::QEntity* rootEntity )
  : BlockBase(),
    mainWindow( mainWindow ),
    tmw( tmw ),
    rootEntity( rootEntity ) {
  widget = new GlobalPlannerToolbar( mainWindow );
  dock = new KDDockWidgets::DockWidget( uniqueName );

  QObject::connect( widget, &GlobalPlannerToolbar::setAPoint, this, &GlobalPlannerLines::setAPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::setBPoint, this, &GlobalPlannerLines::setBPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::setAdditionalPoint, this, &GlobalPlannerLines::setAdditionalPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::snap, this, &GlobalPlannerLines::snap );

  // a point marker -> orange
  {
    aPointEntity = new Qt3DCore::QEntity( rootEntity );

    aPointMesh = new Qt3DExtras::QSphereMesh( aPointEntity );
    aPointMesh->setRadius( .2f );
    aPointMesh->setSlices( 20 );
    aPointMesh->setRings( 20 );

    aPointTransform = new Qt3DCore::QTransform( aPointEntity );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial( aPointEntity );
    material->setDiffuse( QColor( "orange" ) );

    aPointEntity->addComponent( aPointMesh );
    aPointEntity->addComponent( material );
    aPointEntity->addComponent( aPointTransform );
    aPointEntity->setEnabled( false );

    aTextEntity = new Qt3DCore::QEntity( aPointEntity );
    Qt3DExtras::QExtrudedTextMesh* aTextMesh = new Qt3DExtras::QExtrudedTextMesh( aTextEntity );
    aTextMesh->setText( QStringLiteral( "A" ) );
    aTextMesh->setDepth( 0.05f );

    aTextEntity->setEnabled( true );
    aTextTransform = new Qt3DCore::QTransform( aTextEntity );
    aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    aTextTransform->setScale( 2.0f );
    aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    aTextEntity->addComponent( aTextTransform );
    aTextEntity->addComponent( aTextMesh );
    aTextEntity->addComponent( material );
  }

  // b point marker -> purple
  {
    bPointEntity = new Qt3DCore::QEntity( rootEntity );
    bPointMesh = new Qt3DExtras::QSphereMesh( bPointEntity );
    bPointMesh->setRadius( .2f );
    bPointMesh->setSlices( 20 );
    bPointMesh->setRings( 20 );

    bPointTransform = new Qt3DCore::QTransform( bPointEntity );

    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial( bPointEntity );
    material->setDiffuse( QColor( "purple" ) );

    bPointEntity->addComponent( bPointMesh );
    bPointEntity->addComponent( material );
    bPointEntity->addComponent( bPointTransform );
    bPointEntity->setEnabled( false );

    bTextEntity = new Qt3DCore::QEntity( bPointEntity );
    Qt3DExtras::QExtrudedTextMesh* bTextMesh = new Qt3DExtras::QExtrudedTextMesh( bTextEntity );
    bTextMesh->setText( QStringLiteral( "B" ) );
    bTextMesh->setDepth( 0.05f );

    bTextEntity->setEnabled( true );
    bTextTransform = new Qt3DCore::QTransform( bTextEntity );
    bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    bTextTransform->setScale( 2.0f );
    bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    bTextEntity->addComponent( bTextTransform );
    bTextEntity->addComponent( bTextMesh );
    bTextEntity->addComponent( material );
  }

  // test for recording
  {
    m_baseEntity = new Qt3DCore::QEntity( rootEntity );
    m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
    m_baseEntity->addComponent( m_baseTransform );

    m_pointsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity2 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity3 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity4 = new Qt3DCore::QEntity( m_baseEntity );
    m_segmentsEntity->setEnabled( false );
    m_segmentsEntity2->setEnabled( false );
    m_segmentsEntity3->setEnabled( false );

    m_pointsMesh = new BufferMesh( m_pointsEntity );
    m_pointsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
    m_pointsEntity->addComponent( m_pointsMesh );

    m_segmentsMesh = new BufferMesh( m_segmentsEntity );
    m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines );
    m_segmentsEntity->addComponent( m_segmentsMesh );

    m_segmentsMesh2 = new BufferMesh( m_segmentsEntity2 );
    m_segmentsMesh2->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity2->addComponent( m_segmentsMesh2 );

    m_segmentsMesh3 = new BufferMesh( m_segmentsEntity3 );
    m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity3->addComponent( m_segmentsMesh3 );

    m_segmentsMesh4 = new BufferMesh( m_segmentsEntity4 );
    m_segmentsMesh4->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
    m_segmentsEntity4->addComponent( m_segmentsMesh4 );

    m_pointsMaterial = new Qt3DExtras::QPhongMaterial( m_pointsEntity );
    m_segmentsMaterial = new Qt3DExtras::QPhongMaterial( m_segmentsEntity );
    m_segmentsMaterial2 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity2 );
    m_segmentsMaterial3 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity3 );
    m_segmentsMaterial4 = new Qt3DExtras::QPhongMaterial( m_segmentsEntity4 );

    m_pointsMaterial->setAmbient( Qt::yellow );
    m_segmentsMaterial->setAmbient( Qt::green );
    m_segmentsMaterial2->setAmbient( Qt::green );
    m_segmentsMaterial3->setAmbient( Qt::blue );
    m_segmentsMaterial4->setAmbient( Qt::red );

    m_pointsEntity->addComponent( m_pointsMaterial );
    m_segmentsEntity->addComponent( m_segmentsMaterial );
    m_segmentsEntity2->addComponent( m_segmentsMaterial2 );
    m_segmentsEntity3->addComponent( m_segmentsMaterial3 );
    m_segmentsEntity4->addComponent( m_segmentsMaterial4 );
  }

  // create the CGAL worker and move it to it's own thread
  {
    threadForCgalWorker = new CgalThread( this );
    cgalWorker = new CgalWorker();
    cgalWorker->moveToThread( threadForCgalWorker );

    qRegisterMetaType<FieldsOptimitionToolbar::AlphaType>();
    qRegisterMetaType<uint32_t>( "uint32_t" );

//    QObject::connect( this, &GlobalPlanner::requestNewRunNumber, threadForCgalWorker, &CgalThread::requestNewRunNumber );
//    QObject::connect( threadForCgalWorker, &CgalThread::runNumberChanged, this, &GlobalPlanner::setRunNumber );
//    QObject::connect( threadForCgalWorker, &QThread::finished, cgalWorker, &CgalWorker::deleteLater );

//    QObject::connect( this, &GlobalPlanner::requestFieldOptimition, cgalWorker, &CgalWorker::fieldOptimitionWorker );
//    QObject::connect( cgalWorker, &CgalWorker::alphaChanged, this, &GlobalPlanner::alphaChanged );
//    QObject::connect( cgalWorker, &CgalWorker::fieldStatisticsChanged, this, &GlobalPlanner::fieldStatisticsChanged );
//    QObject::connect( cgalWorker, &CgalWorker::alphaShapeFinished, this, &GlobalPlanner::alphaShapeFinished );

    threadForCgalWorker->start();
  }
}

void GlobalPlannerLines::openAbLine() {
  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow,
                                      tr( "Open Saved AB-Line/Curve" ),
                                      dir,
                                      selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;GeoJSON Files (*.geojson)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect to
  // the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile(
        QUrl::fromPercentEncoding(
          fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {

        openAbLineFromFile( loadFile );
      }
    }

    // block all further signals, so no double opening happens
    fileDialog->blockSignals( true );

    fileDialog->deleteLater();
  } );
#else
  QObject::connect( fileDialog, &QFileDialog::fileSelected, mainWindow, [this, fileDialog]( const QString & fileName ) {
    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile( fileName );

      if( !loadFile.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Couldn't open save file.";
      } else {

        openAbLineFromFile( loadFile );
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

void GlobalPlannerLines::openAbLineFromFile( QFile& file ) {
  QByteArray saveData = file.readAll();

  QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
  QJsonObject json = loadDoc.object();

  if( json.contains( QStringLiteral( "type" ) ) && json[QStringLiteral( "type" )] == "FeatureCollection" &&
      json.contains( QStringLiteral( "features" ) ) && json[QStringLiteral( "features" )].isArray() ) {
    QJsonArray featuresArray = json[QStringLiteral( "features" )].toArray();

    for( const auto& feature : qAsConst( featuresArray ) ) {

      QJsonObject featuresObject = feature.toObject();

      if( featuresObject.contains( QStringLiteral( "type" ) ) && featuresObject[QStringLiteral( "type" )] == "Feature" &&
          featuresObject.contains( QStringLiteral( "geometry" ) ) && featuresObject[QStringLiteral( "geometry" )].isObject() ) {
        QJsonObject geometryObject = featuresObject[QStringLiteral( "geometry" )].toObject();

        if( geometryObject.contains( QStringLiteral( "type" ) ) ) {

          // processed field
          if( ( geometryObject[QStringLiteral( "type" )] == "LineString" ) &&
              geometryObject.contains( QStringLiteral( "coordinates" ) ) && geometryObject[QStringLiteral( "coordinates" )].isArray() ) {
            QJsonArray coordinatesArray = geometryObject[QStringLiteral( "coordinates" )].toArray();

            if( coordinatesArray.size() >= 2 ) {
              uint16_t index = 0;

              for( const auto& blockIndex : qAsConst( coordinatesArray ) ) {
                QJsonArray coordinate = blockIndex.toArray();

                if( coordinate.size() >= 2 ) {
                  double x, y, z;
                  double height = 0;

                  if( coordinate.size() >= 3 ) {
                    height = coordinate.at( 2 ).toDouble();
                  };

                  tmw->Forward( coordinate.at( 1 ).toDouble(), coordinate.at( 0 ).toDouble(), height, x, y, z );

                  if( index == 0 ) {
                    aPoint = K::Point_3( x, y, 0 );
                    aPointTransform->setTranslation( convertPoint3ToQVector3D( aPoint ) );
                  }

                  if( index == 1 ) {
                    bPoint = K::Point_3( x, y, 0 );
                    bPointTransform->setTranslation( convertPoint3ToQVector3D( bPoint ) );
                    aPointEntity->setEnabled( true );
                    bPointEntity->setEnabled( true );
                    abSegment = Segment_3( aPoint, bPoint );
                  }
                }

                ++index;
              }
            }
          }
        }
      }
    }
  }

  clearPlan();
  createPlanAB();
}

void GlobalPlannerLines::saveAbLine() {
  if( abSegment.squared_length() > 1 &&
      implementSegment.squared_length() > 1 ) {
    QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
    QString dir;
    QString fileName = QFileDialog::getSaveFileName( mainWindow,
                       tr( "Save AB-Line/Curve" ),
                       dir,
                       tr( "All Files (*);;GeoJSON Files (*.geojson)" ),
                       &selectedFilter );

    if( !fileName.isEmpty() ) {

      QFile saveFile( fileName );

      if( !saveFile.open( QIODevice::WriteOnly ) ) {
        qWarning() << "Couldn't open save file.";
        return;
      }

      saveAbLineToFile( saveFile );
    }
  }
}

void GlobalPlannerLines::saveAbLineToFile( QFile& file ) {
  QJsonObject jsonObject;
  QJsonArray features;

  jsonObject[QStringLiteral( "type" )] = QStringLiteral( "FeatureCollection" );

  // processed field as Polygon
  if( abSegment.squared_length() > 1 &&
      implementSegment.squared_length() > 1 ) {
    QJsonObject feature;

    QJsonObject geometry;

    geometry[QStringLiteral( "type" )] = QStringLiteral( "LineString" );
    QJsonObject properties;

    QJsonArray coordinates;

    {
      double latitude = 0;
      double longitude = 0;
      double height = 0;

      tmw->Reverse( aPoint.x(), aPoint.y(), 0, latitude, longitude, height );

      {
        QJsonArray coordinate;
        coordinate.push_back( longitude );
        coordinate.push_back( latitude );

        coordinates.push_back( coordinate );
      }

      tmw->Reverse( bPoint.x(), bPoint.y(), 0, latitude, longitude, height );

      {
        QJsonArray coordinate;
        coordinate.push_back( longitude );
        coordinate.push_back( latitude );

        coordinates.push_back( coordinate );
      }
    }

    geometry[QStringLiteral( "coordinates" )] = coordinates;

    feature[QStringLiteral( "type" )] = QStringLiteral( "Feature" );
    feature[QStringLiteral( "geometry" )] = geometry;

    properties[QStringLiteral( "name" )] = QStringLiteral( "AB-Line/Curve" );
    feature[QStringLiteral( "properties" )] = properties;

    features.push_back( feature );

    jsonObject[QStringLiteral( "features" )] = features;

    QJsonDocument jsonDocument( jsonObject );
    file.write( jsonDocument.toJson() );
  }
}
