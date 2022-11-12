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

#include <algorithm>

#include <QAction>
#include <QFile>
#include <QMenu>

#include "GlobalPlanner.h"

#include "qneblock.h"
#include "qneport.h"

#include <QScopedPointer>

#include "gui/FieldsOptimitionToolbar.h"
#include "gui/GlobalPlannerToolbar.h"
#include "gui/MyMainWindow.h"

#include "3d/BufferMesh.h"

#include <QFileDialog>

#include "kinematic/CgalWorker.h"
#include "kinematic/cgal.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include "kinematic/Plan.h"
#include "kinematic/PlanGlobal.h"

#include "helpers/GeoJsonHelper.h"
#include "helpers/GeographicConvertionWrapper.h"

#include <CGAL/Aff_transformation_3.h>
using Aff_transformation_3 = CGAL::Aff_transformation_3< Epick >;

GlobalPlanner::GlobalPlanner( const QString&               uniqueName,
                              MyMainWindow*                mainWindow,
                              GeographicConvertionWrapper* tmw,
                              Qt3DCore::QEntity*           rootEntity )
    : tmw( tmw ), mainWindow( mainWindow ), rootEntity( rootEntity ) {
  widget = new GlobalPlannerToolbar( mainWindow );
  dock   = new KDDockWidgets::DockWidget( uniqueName );

  abPolyline = std::make_shared< std::vector< Point_2 > >();

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

    auto* material = new Qt3DExtras::QPhongMaterial( aPointEntity );
    material->setDiffuse( QColor( "orange" ) );

    aPointEntity->addComponent( aPointMesh );
    aPointEntity->addComponent( material );
    aPointEntity->addComponent( aPointTransform );
    aPointEntity->setEnabled( false );

    aTextEntity     = new Qt3DCore::QEntity( aPointEntity );
    auto* aTextMesh = new Qt3DExtras::QExtrudedTextMesh( aTextEntity );
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
    bPointMesh   = new Qt3DExtras::QSphereMesh( bPointEntity );
    bPointMesh->setRadius( .2f );
    bPointMesh->setSlices( 20 );
    bPointMesh->setRings( 20 );

    bPointTransform = new Qt3DCore::QTransform( bPointEntity );

    auto* material = new Qt3DExtras::QPhongMaterial( bPointEntity );
    material->setDiffuse( QColor( "purple" ) );

    bPointEntity->addComponent( bPointMesh );
    bPointEntity->addComponent( material );
    bPointEntity->addComponent( bPointTransform );
    bPointEntity->setEnabled( false );

    bTextEntity     = new Qt3DCore::QEntity( bPointEntity );
    auto* bTextMesh = new Qt3DExtras::QExtrudedTextMesh( bTextEntity );
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
}

GlobalPlanner::~GlobalPlanner() {
  dock->deleteLater();
  widget->deleteLater();
}

void
GlobalPlanner::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    this->position    = toPoint3( position );
    this->orientation = orientation;

    auto quat = toQQuaternion( orientation );
    aPointTransform->setRotation( quat );
    bPointTransform->setRotation( quat );

    auto position2D = to2D( position );

    if( recordContinous ) {
      abPolyline->push_back( position2D );
    }

    //        QElapsedTimer timer;
    //        timer.start();
    plan.expand( position2D );
    //        qDebug() << "Cycle Time plan.expandPlan:" << timer.nsecsElapsed() << "ns";
  }
}

void
GlobalPlanner::setPoseLeftEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    if( options.testFlag( CalculationOption::CalculateLocalOffsets ) && options.testFlag( CalculationOption::NoOrientation ) ) {
      std::cout << "GlobalPlanner::setPoseLeftEdge " << this << ", " << position << std::endl;
      positionLeftEdgeOfImplement = toPoint3( position );

      auto point2D = to2D( position );

      if( implementSegment.source() != point2D ) {
        implementSegment = Segment_2( point2D, implementSegment.target() );
        createPlanAB();
      }
    }
  }
}

void
GlobalPlanner::setPoseRightEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    if( options.testFlag( CalculationOption::CalculateLocalOffsets ) && options.testFlag( CalculationOption::NoOrientation ) ) {
      std::cout << "GlobalPlanner::setPoseRightEdge " << this << ", " << position << std::endl;
      positionRightEdgeOfImplement = toPoint3( position );

      auto point2D = to2D( position );

      if( implementSegment.target() != point2D ) {
        implementSegment = Segment_2( implementSegment.source(), point2D );
        createPlanAB();
      }
    }
  }
}

void
GlobalPlanner::setField( std::shared_ptr< Polygon_with_holes_2 > field ) {
  currentField = field;
}

void
GlobalPlanner::setAPoint() {
  std::cout << "setAPoint " << this << std::endl;
  aPointTransform->setTranslation( toQVector3D( position ) );

  aPointEntity->setEnabled( true );
  bPointEntity->setEnabled( false );
  pointsEntity->setEnabled( false );

  aPoint     = position;
  abPolyline = std::make_shared< std::vector< Point_2 > >();
  abPolyline->push_back( to2D( position ) );
}

void
GlobalPlanner::setBPoint() {
  std::cout << "setBPoint " << this << std::endl;
  bPointTransform->setTranslation( toQVector3D( position ) );
  bPointEntity->setEnabled( true );

  bPoint = position;
  abPolyline->push_back( to2D( position ) );

  abSegment = Segment_3( aPoint, bPoint );

  createPlanAB();
}

void
GlobalPlanner::setAdditionalPoint() {
  std::cout << "setAdditionalPoint " << this << std::endl;
  abPolyline->push_back( to2D( position ) );
  createPlanAB();
}

void
GlobalPlanner::setAdditionalPointsContinous( const bool enabled ) {
  if( recordContinous && !enabled ) {
    createPlanAB();
  }

  recordContinous = enabled;
}

void
GlobalPlanner::snap() {
  snapPlanAB();
}

void
GlobalPlanner::createPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polyline ) {
  Point_2 position2D = to2D( position );

  if( polyline->size() > 2 ) {
    aPointEntity->setEnabled( false );
    bPointEntity->setEnabled( false );
    widget->setToolbarToAdditionalPoint();

    for( const auto& child : qAsConst( pointsEntity->children() ) ) {
      if( auto* childPtr = qobject_cast< Qt3DCore::QEntity* >( child ) ) {
        childPtr->setEnabled( false );
        childPtr->deleteLater();
      }
    }

    for( const auto& point : *polyline ) {
      auto* entity = new Qt3DCore::QEntity( pointsEntity );

      auto* transform = new Qt3DCore::QTransform( entity );
      transform->setTranslation( toQVector3D( point ) );

      entity->addComponent( pointsMaterial );
      entity->addComponent( pointsMesh );
      entity->addComponent( transform );

      entity->setEnabled( true );
    }

    pointsEntity->setEnabled( true );

    plan.resetPlanWith( make_shared< PathPrimitiveSequence >( *polyline, std::sqrt( implementSegment.squared_length() ), true, 0 ) );

    Q_EMIT planChanged( plan );
  }

  if( polyline->size() == 2 ) {
    aPoint    = to3D( polyline->front() );
    bPoint    = to3D( polyline->back() );
    abSegment = Segment_3( aPoint, bPoint );

    aPointTransform->setTranslation( toQVector3D( aPoint ) );
    bPointTransform->setTranslation( toQVector3D( bPoint ) );
    aPointEntity->setEnabled( true );
    bPointEntity->setEnabled( true );

    widget->setToolbarToAdditionalPoint();

    plan.resetPlanWith(
      make_shared< PathPrimitiveLine >( to2D( abSegment ).supporting_line(), std::sqrt( implementSegment.squared_length() ), true, 0 ) );

    Q_EMIT planChanged( plan );
  }

  plan.expand( position2D );
}

void
GlobalPlanner::createPlanAB() {
  std::cout << "GlobalPlanner::createPlanAB " << this << ", " << abSegment.squared_length() << ", " << implementSegment.squared_length()
            << std::endl;
  if( abSegment.squared_length() > 0.25 && implementSegment.squared_length() > 1 ) {
    std::cout << "GlobalPlanner::createPlanAB 2 " << this << std::endl;

    Point_2 position2D = to2D( position );

    if( abPolyline->size() == 2 ) {
      std::cout << "GlobalPlanner::createPlanAB 3 " << this << std::endl;
      createPlanPolyline( abPolyline );
    }

    if( abPolyline->size() > 2 ) {
      std::cout << "GlobalPlanner::createPlanAB 4 " << this << std::endl;
      planOptimitionController = new PlanOptimitionController( abPolyline, 0.1 );
      QObject::connect(
        planOptimitionController, &PlanOptimitionController::simplifyPolylineResult, this, &GlobalPlanner::createPlanPolyline );

      planOptimitionController->run();
    }
  }
}

void
GlobalPlanner::snapPlanAB() {
  if( abSegment.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    double xte              = 0;
    auto   nearestPrimitive = plan.getNearestPrimitive( position2D, xte );
    xte                     = std::sqrt( xte );
    xte *= ( *nearestPrimitive )->offsetSign( position2D );
    double angleDegrees = ( *nearestPrimitive )->angleAtPointDegrees( position2D );

    auto offsetVector2D = polarOffsetDegrees( M_PI + angleDegrees, xte );
    auto offsetVector3D = to3D( offsetVector2D );

    Aff_transformation_2 transformation2D( CGAL::TRANSLATION, -offsetVector2D );
    Aff_transformation_3 transformation3D( CGAL::TRANSLATION, -offsetVector3D );

    aPoint    = aPoint.transform( transformation3D );
    bPoint    = bPoint.transform( transformation3D );
    abSegment = Segment_3( aPoint, bPoint );

    aPointTransform->setTranslation( toQVector3D( aPoint ) );
    bPointTransform->setTranslation( toQVector3D( bPoint ) );

    //    QElapsedTimer timer;
    //    timer.start();
    plan.transform( transformation2D );
    //    qDebug() << "Cycle Time Snap:" << timer.nsecsElapsed() << "ns";
  }
}

void
GlobalPlanner::openAbLine() {
  QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
  QString dir;

  auto* fileDialog = new QFileDialog( mainWindow, tr( "Open Saved AB-Line/Curve" ), dir, selectedFilter );
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
        openAbLineFromFile( loadFile );
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

void
GlobalPlanner::openAbLineFromFile( QFile& file ) {
  auto geoJsonHelper = GeoJsonHelper( file );
  geoJsonHelper.print();

  for( const auto& member : geoJsonHelper.members ) {
    switch( member.first ) {
      case GeoJsonHelper::GeometryType::LineString: {
        abPolyline = std::make_shared< std::vector< Point_2 > >();
        auto index = uint16_t( 0 );

        for( const auto& point : std::get< GeoJsonHelper::LineStringType >( member.second ) ) {
          auto tmwPoint = toPoint3( tmw->Forward( point ) );

          if( index == 0 ) {
            aPoint = tmwPoint;
            aPointTransform->setTranslation( toQVector3D( tmwPoint ) );
          }

          if( index == 1 ) {
            bPoint = tmwPoint;
            bPointTransform->setTranslation( toQVector3D( tmwPoint ) );
            aPointEntity->setEnabled( true );
            bPointEntity->setEnabled( true );
            abSegment = Segment_3( aPoint, tmwPoint );
          }

          abPolyline->push_back( to2D( tmwPoint ) );

          ++index;
        }

      } break;

      default:
        break;
    }
  }

  createPlanAB();
}

void
GlobalPlanner::newField() {
  widget->resetToolbar();
  abPolyline = std::make_shared< std::vector< Point_2 > >();
}

void
GlobalPlanner::saveAbLine() {
  if( abSegment.squared_length() > 1 && implementSegment.squared_length() > 1 ) {
    QString selectedFilter = QStringLiteral( "GeoJSON Files (*.geojson)" );
    QString dir;
    QString fileName = QFileDialog::getSaveFileName(
      mainWindow, tr( "Save AB-Line/Curve" ), dir, tr( "All Files (*);;GeoJSON Files (*.geojson)" ), &selectedFilter );

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

void
GlobalPlanner::saveAbLineToFile( QFile& file ) {
  if( abSegment.squared_length() > 1 && implementSegment.squared_length() > 1 ) {
    auto geoJsonHelper = GeoJsonHelper();
    auto lineString    = GeoJsonHelper::LineStringType();

    for( const auto& point : *abPolyline ) {
      lineString.emplace_back( tmw->Reverse( toEigenVector( point ) ) );
    }

    geoJsonHelper.addFeature( GeoJsonHelper::GeometryType::LineString, std::move( lineString ) );

    geoJsonHelper.save( file );
  }
}

void
GlobalPlanner::setPlannerSettings( const int pathsInReserve, const double maxDeviation ) {
  plan.pathsInReserve = pathsInReserve;
  this->maxDeviation  = maxDeviation;

  createPlanAB();
}

void
GlobalPlanner::setPassSettings( const int forwardPasses, const int reversePasses, const bool startRight, const bool mirror ) {
  if( ( forwardPasses == 0 || reversePasses == 0 ) ) {
    this->forwardPasses = 0;
    this->reversePasses = 0;
  } else {
    this->forwardPasses = forwardPasses;
    this->reversePasses = reversePasses;
  }

  this->startRight = startRight;
  this->mirror     = mirror;
}

void
GlobalPlanner::setPassNumberTo( const int ) {}

GlobalPlannerFactory::GlobalPlannerFactory( QThread*                     thread,
                                            MyMainWindow*                mainWindow,
                                            KDDockWidgets::Location      location,
                                            QMenu*                       menu,
                                            GeographicConvertionWrapper* tmw,
                                            Qt3DCore::QEntity*           rootEntity )
    : BlockFactory( thread ), mainWindow( mainWindow ), location( location ), menu( menu ), rootEntity( rootEntity ), tmw( tmw ) {
  qRegisterMetaType< Plan >();
  qRegisterMetaType< PlanGlobal >();
}

QNEBlock*
GlobalPlannerFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* object = new GlobalPlanner( getNameOfFactory() + QString::number( id ), mainWindow, tmw, rootEntity );
  auto* b      = createBaseBlock( scene, object, id, true );
  object->moveToThread( thread );

  object->dock->setTitle( QStringLiteral( "Global Planner" ) );
  object->dock->setWidget( object->widget );

  menu->addAction( object->dock->toggleAction() );

  mainWindow->addDockWidget( object->dock, location );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Left Edge" ), QLatin1String( SLOT( setPoseLeftEdge( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose Right Edge" ), QLatin1String( SLOT( setPoseRightEdge( POSE_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Field" ), QLatin1String( SLOT( setField( std::shared_ptr< Polygon_with_holes_2 > ) ) ) );

  b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );

  return b;
}
