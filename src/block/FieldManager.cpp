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
#include <QScopedPointer>

#include <QFileDialog>

#include "../cgal.h"

FieldManager::FieldManager( QWidget* mainWindow, Qt3DCore::QEntity* rootEntity, GeographicConvertionWrapper* tmw )
  : BlockBase(),
    mainWindow( mainWindow ), tmw( tmw ) {

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
    m_segmentsMesh->setPrimitiveType( Qt3DRender::QGeometryRenderer::LineStrip );
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

  // create the CGAL worker and move to it's own thread
  {
    threadForCgalWorker = new CgalThread( this );
    cgalWorker = new CgalWorker();
    cgalWorker->moveToThread( threadForCgalWorker );

    qRegisterMetaType<FieldsOptimitionToolbar::AlphaType>();
    qRegisterMetaType<uint32_t>( "uint32_t" );
    qRegisterMetaType<std::shared_ptr<Polygon_with_holes_2>>( /*"std::shared_ptr<Polygon_with_holes_2>"*/ );

    QObject::connect( this, &FieldManager::requestNewRunNumber, threadForCgalWorker, &CgalThread::requestNewRunNumber );
    QObject::connect( threadForCgalWorker, &CgalThread::runNumberChanged, this, &FieldManager::setRunNumber );
    QObject::connect( threadForCgalWorker, &QThread::finished, cgalWorker, &CgalWorker::deleteLater );

    QObject::connect( this, &FieldManager::requestFieldOptimition, cgalWorker, &CgalWorker::fieldOptimitionWorker );
    QObject::connect( cgalWorker, &CgalWorker::alphaChanged, this, &FieldManager::alphaChanged );
    QObject::connect( cgalWorker, &CgalWorker::fieldStatisticsChanged, this, &FieldManager::fieldStatisticsChanged );
    QObject::connect( cgalWorker, &CgalWorker::alphaShapeFinished, this, &FieldManager::alphaShapeFinished );

    threadForCgalWorker->start();
  }
}

void FieldManager::alphaShape() {
  QElapsedTimer timer;
  timer.start();

  // you need at least 3 points for area
  if( points.size() >= 3 ) {

    // make a 2D copy of the recorded points
    auto pointsCopy2D = new std::vector<K::Point_2>();
    pointsCopy2D->reserve( points.size() );

    for( const auto& point : points ) {
      pointsCopy2D->emplace_back( point.x(), point.y() );
    }

    emit requestNewRunNumber();

    emit requestFieldOptimition( runNumber,
                                 pointsCopy2D,
                                 alphaType,
                                 customAlpha,
                                 maxDeviation,
                                 distanceBetweenConnectPoints );
  }

  qDebug() << "FieldManager::alphaShape: " << timer.nsecsElapsed() << "ns";
}

void FieldManager::openField() {
  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow,
                                      tr( "Open Saved Config" ),
                                      dir,
                                      selectedFilter );
  fileDialog->setFileMode( QFileDialog::ExistingFile );
  fileDialog->setNameFilter( tr( "All Files (*);;GeoJSON Files (*.geojson)" ) );

  // connect the signal QFileDialog::urlSelected to a lambda, which opens the file.
  // this is needed, as the file dialog on android is asynchonous, so you have to connect to
  // the signals instead of using the static functions for the dialogs
#ifdef Q_OS_ANDROID
  QObject::connect( fileDialog, &QFileDialog::urlSelected, this, [this, fileDialog]( QUrl fileName ) {
    qDebug() << "QFileDialog::urlSelected QUrl" << fileName << fileName.toDisplayString() << fileName.toLocalFile();

    if( !fileName.isEmpty() ) {
      // some string wrangling on android to get the native file name
      QFile loadFile(
        QUrl::fromPercentEncoding(
          fileName.toString().split( QStringLiteral( "%3A" ) ).at( 1 ).toUtf8() ) );

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
  QObject::connect( fileDialog, &QFileDialog::fileSelected, mainWindow, [this, fileDialog]( const QString & fileName ) {
    qDebug() << "QFileDialog::fileSelected QString" << fileName;

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

void FieldManager::openFieldFromFile( QFile& file ) {

  QByteArray saveData = file.readAll();

  QJsonDocument loadDoc( QJsonDocument::fromJson( saveData ) );
  QJsonObject json = loadDoc.object();

  bool newField = false;
  bool newRawPoints = false;

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
          if( ( geometryObject[QStringLiteral( "type" )] == "Polygon" || geometryObject[QStringLiteral( "type" )] == "MultiPolygon" ) &&
              geometryObject.contains( QStringLiteral( "coordinates" ) ) && geometryObject[QStringLiteral( "coordinates" )].isArray() ) {
            QJsonArray coordinatesArray = geometryObject[QStringLiteral( "coordinates" )].toArray();

            if( coordinatesArray.size() >= 1 ) {
              QJsonArray coordinateArray;

              if( geometryObject[QStringLiteral( "type" )] == "MultiPolygon" ) {
                coordinateArray = coordinatesArray.first().toArray().first().toArray();
              } else {
                coordinateArray = coordinatesArray.first().toArray();
              }

              QVector<QVector3D> positions;
              Polygon_2 poly;

              for( const auto& blockIndex : qAsConst( coordinateArray ) ) {
                QJsonArray coordinate = blockIndex.toArray();

                if( coordinate.size() >= 2 ) {
                  double x, y, z;

                  tmw->Forward( coordinate.at( 1 ).toDouble(), coordinate.at( 0 ).toDouble(), x, y, z );
                  positions.push_back( QVector3D( x, y, z ) );
                  poly.push_back( K::Point_2( x, y ) );
                }
              }

              currentField = std::make_shared<Polygon_with_holes_2>( poly );

              const auto& outerPoly = currentField->outer_boundary();
              emit pointsInFieldBoundaryChanged( outerPoly.size() );

              newField = true;

              m_segmentsMesh2->bufferUpdate( positions );
              m_segmentsEntity2->setEnabled( true );

              emit fieldChanged(currentField);
            }
          }

          // raw points
          if( geometryObject[QStringLiteral( "type" )] == "MultiPoint" &&
              geometryObject.contains( QStringLiteral( "coordinates" ) ) && geometryObject[QStringLiteral( "coordinates" )].isArray() ) {
            QJsonArray coordinatesArray = geometryObject[QStringLiteral( "coordinates" )].toArray();

            if( coordinatesArray.size() >= 1 ) {
              QVector<QVector3D> positions;
              points.clear();

              for( const auto& blockIndex : qAsConst( coordinatesArray ) ) {
                QJsonArray coordinate = blockIndex.toArray();

                if( coordinate.size() >= 2 ) {
                  double x, y, z;
                  double height = 0;

                  if( coordinate.size() >= 3 ) {
                    height = coordinate.at( 2 ).toDouble();
                  };

                  tmw->Forward( coordinate.at( 1 ).toDouble(), coordinate.at( 0 ).toDouble(), height, x, y, z );

                  positions.push_back( QVector3D( x, y, z ) );

                  points.emplace_back( K::Point_3( x, y, z ) );
                }
              }

              m_segmentsMesh3->bufferUpdate( positions );
              m_segmentsMesh3->setPrimitiveType( Qt3DRender::QGeometryRenderer::Points );
              m_segmentsEntity3->setEnabled( true );

              emit pointsGeneratedForFieldBoundaryChanged( 0 );
              emit pointsRecordedChanged( points.size() );

              newRawPoints = true;
            }
          }
        }
      }
    }
  }

  // if a file is loaded with only raw points, then recalculate() to set it as new boundary
  if( newRawPoints && ! newField ) {
    recalculateField();
  }
}

void FieldManager::saveField() {
  if( currentField || !points.empty() ) {
    QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
    QString dir;
    QString fileName = QFileDialog::getSaveFileName( mainWindow,
                       tr( "Open Saved Config" ),
                       dir,
                       tr( "All Files (*);;GeoJSON Files (*.geojson)" ),
                       &selectedFilter );

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

void FieldManager::saveFieldToFile( QFile& file ) {
  QJsonObject jsonObject;
  QJsonArray features;

  jsonObject[QStringLiteral( "type" )] = QStringLiteral( "FeatureCollection" );

  QJsonObject properties;
  properties[QStringLiteral( "alpha" )] = currentAlpha;
  properties[QStringLiteral( "connect-points-distance" )] = distanceBetweenConnectPoints;
  properties[QStringLiteral( "simplification-max-deviation" )] = maxDeviation;


  // processed field as Polygon
  if( currentField ) {
    QJsonObject feature;

    QJsonObject geometry;

    geometry[QStringLiteral( "type" )] = QStringLiteral( "Polygon" );

    QJsonArray coordinatesDummy;
    QJsonArray coordinates;

    typedef Polygon_2::Vertex_iterator VertexIterator;

    auto outerBoundary = currentField->outer_boundary();
//    outerBoundary.reverse_orientation();

    for( VertexIterator vi = outerBoundary.vertices_begin(),
         end = outerBoundary.vertices_end();
         vi != end; ++vi ) {

      double latitude = 0;
      double longitude = 0;
      double height = 0;
      tmw->Reverse( vi->x(), vi->y(), 0, latitude, longitude, height );

      QJsonArray coordinate;
      coordinate.push_back( longitude );
      coordinate.push_back( latitude );

      coordinates.push_back( coordinate );
    }


    // add the first point again
    {
      VertexIterator vi = outerBoundary.vertices_begin();
      double latitude = 0;
      double longitude = 0;
      double height = 0;

      tmw->Reverse( vi->x(), vi->y(), 0, latitude, longitude, height );

      QJsonArray coordinate;
      coordinate.push_back( longitude );
      coordinate.push_back( latitude );

      coordinates.push_back( coordinate );
    }

    coordinatesDummy.push_back( coordinates );
    geometry[QStringLiteral( "coordinates" )] = coordinatesDummy;

    feature[QStringLiteral( "type" )] = QStringLiteral( "Feature" );
    feature[QStringLiteral( "geometry" )] = geometry;

    properties[QStringLiteral( "name" )] = QStringLiteral( "processed polygon" );
    feature[QStringLiteral( "properties" )] = properties;

    features.push_back( feature );
  }

  // recorded points as MultiPoint
  if( !points.empty() ) {
    QJsonObject feature;

    QJsonObject geometry;

    geometry[QStringLiteral( "type" )] = QStringLiteral( "MultiPoint" );

    QJsonArray coordinates;

    for( const K::Point_3& point : points ) {

      double latitude = 0;
      double longitude = 0;
      double height = 0;
      tmw->Reverse( point.x(), point.y(), point.z(), latitude, longitude, height );

      QJsonArray coordinate;
      coordinate.push_back( longitude );
      coordinate.push_back( latitude );
      coordinate.push_back( height );

      coordinates.push_back( coordinate );
    }

    geometry[QStringLiteral( "coordinates" )] = coordinates;

    feature[QStringLiteral( "type" )] = QStringLiteral( "Feature" );
    feature[QStringLiteral( "geometry" )] = geometry;

    properties[QStringLiteral( "name" )] = QStringLiteral( "raw points" );
    feature[QStringLiteral( "properties" )] = properties;

    features.push_back( feature );
  }

  jsonObject[QStringLiteral( "features" )] = features;

  QJsonDocument jsonDocument( jsonObject );
  file.write( jsonDocument.toJson() );
}

void FieldManager::alphaShapeFinished( std::shared_ptr<Polygon_with_holes_2> field, double alpha ) {
  currentField = field;

  qDebug() << "FieldManager::alphaShapeFinished" << field.get() << alpha;

  QVector<QVector3D> meshSegmentPoints;
  typedef Polygon_2::Vertex_iterator VertexIterator;

  for( VertexIterator vi = field->outer_boundary().vertices_begin(),
       end = field->outer_boundary().vertices_end();
       vi != end; ++vi ) {
    meshSegmentPoints << QVector3D( float( vi->x() ), float( vi->y() ), 0.1f );
  }

  meshSegmentPoints << meshSegmentPoints.first();
  m_segmentsMesh4->bufferUpdate( meshSegmentPoints );

  emit fieldChanged(currentField);
}
