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

#include "GlobalPlanner.h"
#include <QScopedPointer>

#include <QFileDialog>

#include "../cgal.h"
#include "../kinematic/CgalWorker.h"

GlobalPlanner::GlobalPlanner( const QString& uniqueName, MyMainWindow* mainWindow, GeographicConvertionWrapper* tmw, Qt3DCore::QEntity* rootEntity )
  : BlockBase(),
    tmw( tmw ),
    mainWindow( mainWindow ),
    rootEntity( rootEntity ) {
  widget = new GlobalPlannerToolbar( mainWindow );
  dock = new KDDockWidgets::DockWidget( uniqueName );

  QObject::connect( widget, &GlobalPlannerToolbar::setAPoint, this, &GlobalPlanner::setAPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::setBPoint, this, &GlobalPlanner::setBPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::setAdditionalPoint, this, &GlobalPlanner::setAdditionalPoint );
  QObject::connect( widget, &GlobalPlannerToolbar::setAdditionalPointsContinous, this, &GlobalPlanner::setAdditionalPointsContinous );
  QObject::connect( widget, &GlobalPlannerToolbar::snap, this, &GlobalPlanner::snap );

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

  {
    pointsEntity = new Qt3DCore::QEntity( rootEntity );

    pointsMesh = new Qt3DExtras::QSphereMesh( bPointEntity );
    pointsMesh->setRadius( .2f );
    pointsMesh->setSlices( 20 );
    pointsMesh->setRings( 20 );

    pointsMaterial = new Qt3DExtras::QPhongMaterial( bPointEntity );
    pointsMaterial->setDiffuse( QColor( "purple" ) );
  }

  {
    threadForCgalWorker = new CgalThread( this );
    cgalWorker = new CgalWorker();
    cgalWorker->moveToThread( threadForCgalWorker );
    threadForCgalWorker->start();
    QObject::connect( this, &GlobalPlanner::requestPolylineSimplification, cgalWorker, &CgalWorker::simplifyPolyline );

    QObject::connect( cgalWorker, &CgalWorker::simplifyPolylineResult, this, &GlobalPlanner::createPlanPolyline );
  }
}

void GlobalPlanner::createPlanPolyline( std::vector<Point_2>* polylinePtr ) {
  std::unique_ptr<std::vector<Point_2>> polyline( polylinePtr );
  Point_2 position2D = to2D( position );

  if( polyline->size() > 2 ) {
    aPointEntity->setEnabled( false );
    bPointEntity->setEnabled( false );
    widget->setToolbarToAdditionalPoint();

    for( const auto& child : qAsConst( pointsEntity->children() ) ) {
      if( auto childPtr = qobject_cast<Qt3DCore::QEntity*>( child ) ) {
        childPtr->setEnabled( false );
        childPtr->deleteLater();
      }
    }

    for( const auto& point : *polyline ) {
      auto entity = new Qt3DCore::QEntity( pointsEntity );

      auto transform = new Qt3DCore::QTransform( entity );
      transform->setTranslation( convertPoint2ToQVector3D( point ) );

      entity->addComponent( pointsMaterial );
      entity->addComponent( pointsMesh );
      entity->addComponent( transform );

      entity->setEnabled( true );
    }

    pointsEntity->setEnabled( true );

    plan.resetPlanWith( make_shared<PathPrimitiveSequence>(
                          *polyline,
                          std::sqrt( implementSegment.squared_length() ),
                          true,
                          0 ) );


    emit planChanged( plan );
  }

  if( polyline->size() == 2 ) {
    aPoint = to3D( polyline->front() );
    bPoint = to3D( polyline->back() );
    abSegment = Segment_3( aPoint, bPoint );

    aPointTransform->setTranslation( convertPoint3ToQVector3D( aPoint ) );
    bPointTransform->setTranslation( convertPoint3ToQVector3D( bPoint ) );
    aPointEntity->setEnabled( true );
    bPointEntity->setEnabled( true );

    widget->setToolbarToAdditionalPoint();

    plan.resetPlanWith( make_shared<PathPrimitiveLine>(
                          to2D( abSegment ).supporting_line(),
                          std::sqrt( implementSegment.squared_length() ),
                          true,
                          0 ) );

    emit planChanged( plan );
  }

  plan.expand( position2D );

}

void GlobalPlanner::createPlanAB() {
  if( abSegment.squared_length() > 1 &&
      implementSegment.squared_length() > 1 ) {

    Point_2 position2D = to2D( position );

    if( abPolyline.size() == 2 ) {
      plan.resetPlanWith( make_shared<PathPrimitiveLine>(
                            to2D( abSegment ).supporting_line(),
                            std::sqrt( implementSegment.squared_length() ),
                            true,
                            0 ) );
      plan.expand( position2D );

      emit planChanged( plan );
    }

    if( abPolyline.size() > 2 ) {
      emit requestPolylineSimplification( &abPolyline, 0.1, true );
    }
  }
}

void GlobalPlanner::snapPlanAB() {
  if( abSegment.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    double xte = 0;
    auto nearestPrimitive = plan.getNearestPrimitive( position2D, xte );
    xte = std::sqrt( xte );
    xte *= ( *nearestPrimitive )->offsetSign( position2D );
    double angleDegrees = ( *nearestPrimitive )->angleAtPointDegrees( position2D );

    auto offsetVector2D = polarOffsetDegrees( M_PI + angleDegrees, xte );
    auto offsetVector3D = to3D( offsetVector2D );

    Aff_transformation_2 transformation2D( CGAL::TRANSLATION, -offsetVector2D );
    Aff_transformation_3 transformation3D( CGAL::TRANSLATION, -offsetVector3D );

    aPoint = aPoint.transform( transformation3D );
    bPoint = bPoint.transform( transformation3D );
    abSegment = Segment_3( aPoint, bPoint );

    aPointTransform->setTranslation( convertPoint3ToQVector3D( aPoint ) );
    bPointTransform->setTranslation( convertPoint3ToQVector3D( bPoint ) );

//    QElapsedTimer timer;
//    timer.start();
    plan.transform( transformation2D );
//    qDebug() << "Cycle Time Snap:" << timer.nsecsElapsed() << "ns";
  }
}

void GlobalPlanner::openAbLine() {
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

void GlobalPlanner::openAbLineFromFile( QFile& file ) {
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

              abPolyline.clear();

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

                  abPolyline.push_back( Point_2( x, y ) );
                }

                ++index;
              }
            }
          }
        }
      }
    }
  }

  createPlanAB();
}

void GlobalPlanner::saveAbLine() {
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

void GlobalPlanner::saveAbLineToFile( QFile& file ) {
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

      for( const auto& point : abPolyline ) {
        tmw->Reverse( point.x(), point.y(), 0, latitude, longitude, height );

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
