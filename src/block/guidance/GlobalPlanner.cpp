// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>

#include <QAction>
#include <QFile>
#include <QMenu>

#include "GlobalPlanner.h"

#include <QScopedPointer>

#include "gui/MyMainWindow.h"
#include "gui/OpenSaveHelper.h"
#include "gui/toolbar/FieldsOptimitionToolbar.h"
#include "gui/toolbar/GlobalPlannerToolbar.h"

#include "block/graphical/GlobalPlannerModel.h"

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

#include "block/graphical/PathPlannerModel.h"

#include "helpers/BlocksManager.h"
#include "helpers/GeoJsonHelper.h"
#include "helpers/GeographicConvertionWrapper.h"

#include <CGAL/Aff_transformation_3.h>
using Aff_transformation_3 = CGAL::Aff_transformation_3< Epick >;

GlobalPlanner::GlobalPlanner( const QString&               uniqueName,
                              MyMainWindow*                mainWindow,
                              GeographicConvertionWrapper* tmw,
                              const BlockBaseId            idHint,
                              const bool                   systemBlock,
                              const QString                type )
    : BlockBase( idHint, systemBlock, type ), mainWindow( mainWindow ), tmw( tmw ) {
  widget = new GlobalPlannerToolbar( mainWindow );
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );

  abPolyline = std::make_shared< std::vector< Point_2 > >();

  openSaveHelper = new OpenSaveHelper( "Open A/B-Line", "GeoJSON Files (*.geojson)", mainWindow );

  QObject::connect( openSaveHelper, &OpenSaveHelper::openFile, this, &GlobalPlanner::openAbLineFromString );
  QObject::connect( openSaveHelper, &OpenSaveHelper::saveFile, this, &GlobalPlanner::saveAbLineToString );

  QObject::connect( widget, &GlobalPlannerToolbar::setAPoint, this, &GlobalPlanner::aPointSet );
  QObject::connect( widget, &GlobalPlannerToolbar::setBPoint, this, &GlobalPlanner::bPointSet );
  QObject::connect( widget, &GlobalPlannerToolbar::setAdditionalPoint, this, &GlobalPlanner::additionalPointSet );
  QObject::connect( widget, &GlobalPlannerToolbar::setAdditionalPointsContinous, this, &GlobalPlanner::additionalPointsContinousSet );
  QObject::connect( widget, &GlobalPlannerToolbar::snap, this, &GlobalPlanner::snap );
  QObject::connect( this, &GlobalPlanner::setToolbarToAdditionalPoint, widget, &GlobalPlannerToolbar::toolbarToAdditionalPointSet );
  QObject::connect( this, &GlobalPlanner::resetToolbar, widget, &GlobalPlannerToolbar::resetToolbar );
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

    auto position2D = to2D( position );

    if( recordContinous ) {
      abPolyline->push_back( position2D );
    }

    plan.expand( position2D );

    double distance         = 0;
    auto   nearestPrimitive = plan.getNearestPrimitive( to2D( position ), distance, &lastNearestPrimitive );

    // this generates a new plan with the least amount of primitives possible: the diagonal distance of the view rect is divided by the
    // implement width, which gives the maximum new primitive number. Then the plan is filled: try to add the half of this number on each
    // side of the nearest primitive
    if( nearestPrimitive != plan.plan->cend() && nearestPrimitive != lastNearestPrimitive ) {
      int64_t distanceToFront = std::distance( plan.plan->cbegin(), nearestPrimitive );
      int64_t distanceToEnd   = std::distance( nearestPrimitive, plan.plan->cend() );

      int64_t implementNumInViewRectWorstCase =
        std::sqrt( 2. * std::pow( pathPlannerModel->viewBox, 2 ) ) / std::sqrt( implementSegment.squared_length() );

      if( ( distanceToFront + distanceToEnd ) < implementNumInViewRectWorstCase ) {
        Q_EMIT planChangedForModel( plan );
      } else {
        auto distToBeginIterator = std::min( distanceToFront, implementNumInViewRectWorstCase / 2 );
        auto distToEndIterator   = std::min( distanceToEnd, implementNumInViewRectWorstCase / 2 );

        auto cbegin = nearestPrimitive;
        auto cend   = nearestPrimitive;

        std::advance( cbegin, -distToBeginIterator );
        std::advance( cend, distToEndIterator );

        planForModel.plan->clear();
        std::copy( cbegin, cend, std::back_inserter( *( planForModel.plan ) ) );
        Q_EMIT planChangedForModel( planForModel );
      }

      lastNearestPrimitive = nearestPrimitive;
    }
  }
}

void
GlobalPlanner::setPoseLeftEdge( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    if( options.testFlag( CalculationOption::CalculateLocalOffsets ) && options.testFlag( CalculationOption::NoOrientation ) ) {
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
GlobalPlanner::aPointSet() {
  aPoint     = position;
  abPolyline = std::make_shared< std::vector< Point_2 > >();
  abPolyline->push_back( to2D( position ) );

  Q_EMIT planPolylineChanged( abPolyline );
}

void
GlobalPlanner::bPointSet() {
  bPoint = position;
  abPolyline->push_back( to2D( position ) );

  abSegment = Segment_3( aPoint, bPoint );

  createPlanAB();
}

void
GlobalPlanner::additionalPointSet() {
  abPolyline->push_back( to2D( position ) );
  createPlanAB();
}

void
GlobalPlanner::additionalPointsContinousSet( const bool enabled ) {
  if( recordContinous && !enabled ) {
    createPlanAB();
  }

  recordContinous = enabled;

  Q_EMIT planPolylineChanged( abPolyline );
}

void
GlobalPlanner::snap() {
  snapPlanAB();
}

void
GlobalPlanner::createPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polyline ) {
  Point_2 position2D = to2D( position );

  if( polyline->size() > 2 ) {
    Q_EMIT setToolbarToAdditionalPoint();

    plan.resetPlanWith( make_shared< PathPrimitiveSequence >( *polyline, std::sqrt( implementSegment.squared_length() ), true, 0 ) );

    lastNearestPrimitive = plan.plan->cend();
    Q_EMIT planChanged( plan );
    Q_EMIT planPolylineChanged( polyline );
  }

  if( polyline->size() == 2 ) {
    aPoint    = to3D( polyline->front() );
    bPoint    = to3D( polyline->back() );
    abSegment = Segment_3( aPoint, bPoint );

    Q_EMIT setToolbarToAdditionalPoint();

    plan.resetPlanWith(
      make_shared< PathPrimitiveLine >( to2D( abSegment ).supporting_line(), std::sqrt( implementSegment.squared_length() ), true, 0 ) );

    lastNearestPrimitive = plan.plan->cend();
    Q_EMIT planChanged( plan );
    Q_EMIT planPolylineChanged( polyline );
  }

  plan.expand( position2D );
}

void
GlobalPlanner::createPlanAB() {
  if( ( ( abPolyline->size() >= 2 ) && ( Segment_2( abPolyline->front(), abPolyline->back() ).squared_length() > 0.25 ) ) &&
      implementSegment.squared_length() > 1 ) {
    Point_2 position2D = to2D( position );

    if( abPolyline->size() == 2 ) {
      createPlanPolyline( abPolyline );
    }

    if( abPolyline->size() > 2 ) {
      planOptimitionController = new PlanOptimitionController( abPolyline, 0.1 );
      QObject::connect(
        planOptimitionController, &PlanOptimitionController::simplifyPolylineResult, this, &GlobalPlanner::createPlanPolyline );

      planOptimitionController->run();
    }
  }
}

void
GlobalPlanner::snapPlanAB() {
  if( !plan.plan->empty() ) {
    Point_2 position2D = to2D( position );

    double xte              = 0;
    auto   nearestPrimitive = plan.getNearestPrimitive( position2D, xte, &lastNearestPrimitive );
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

    for( auto& point : *abPolyline ) {
      point = point.transform( transformation2D );
    }

    Q_EMIT planPolylineChanged( abPolyline );

    //    QElapsedTimer timer;
    //    timer.start();
    plan.transform( transformation2D );
    //    qDebug() << "Cycle Time Snap:" << timer.nsecsElapsed() << "ns";
  }
}

void
GlobalPlanner::openAbLineFromString( const QString& fileName ) {
  if( !fileName.isEmpty() ) {
    // some string wrangling on android to get the native file name
    QFile loadFile( fileName );

    if( !loadFile.open( QIODevice::ReadOnly ) ) {
      qWarning() << "Couldn't open save file.";
    } else {
      openAbLineFromFile( loadFile );
    }
  }
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
          }

          if( index == 1 ) {
            bPoint    = tmwPoint;
            abSegment = Segment_3( aPoint, tmwPoint );
          }

          abPolyline->push_back( to2D( tmwPoint ) );

          ++index;
        }

        Q_EMIT planPolylineChanged( abPolyline );

      } break;

      default:
        break;
    }
  }

  createPlanAB();
}

void
GlobalPlanner::newAbLine() {
  Q_EMIT resetToolbar();
  abPolyline = std::make_shared< std::vector< Point_2 > >();
}

void
GlobalPlanner::saveAbLineToString( QString fileName ) {
  if( !fileName.isEmpty() ) {
    // some string wrangling on android to get the native file name
    QFile saveFile( fileName );

    if( !saveFile.open( QIODevice::WriteOnly ) ) {
      qWarning() << "Couldn't open save file.";
    } else {
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
      lineString.push_back( tmw->Reverse( toEigenVector( point ) ) );
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
    : BlockFactory( thread, true ), mainWindow( mainWindow ), location( location ), menu( menu ), tmw( tmw ), rootEntity( rootEntity ) {
  qRegisterMetaType< Plan >();
  qRegisterMetaType< PlanGlobal >();
  qRegisterMetaType< const Plan >();
  qRegisterMetaType< const PlanGlobal >();
  qRegisterMetaType< const Plan& >();
  qRegisterMetaType< const PlanGlobal& >();

  typeColor = TypeColor::Arithmetic;
}

std::unique_ptr< BlockBase >
GlobalPlannerFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< GlobalPlanner >( idHint, getNameOfFactory() + QString::number( idHint ), mainWindow, tmw );

  auto pathPlannerModelId =
    blocksManager.moveObjectToManager( std::make_unique< PathPlannerModel >( rootEntity, 0, false, "Global Plan" ) );

  auto* pathPlannerModel = static_cast< PathPlannerModel* >( blocksManager.getBlock( pathPlannerModelId ) );
  obj->addAdditionalObject( pathPlannerModel );
  obj->pathPlannerModel = pathPlannerModel;

  QObject::connect( obj.get(), &GlobalPlanner::planChanged, pathPlannerModel, &PathPlannerModel::setPlan );

  obj->dock->setTitle( QStringLiteral( "Global Planner" ) );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  mainWindow->addDockWidget( obj->dock, location );

  auto* globalPlannerModel = new GlobalPlannerModel( rootEntity, 0, true, "GlobalPlannerModel" );
  obj->addAdditionalObject( globalPlannerModel );

  QObject::connect( obj.get(), &GlobalPlanner::planPolylineChanged, globalPlannerModel, &GlobalPlannerModel::showPlanPolyline );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Pose" ), pathPlannerModel, QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ), BlockPort::Flag::None );
  obj->addInputPort(
    QStringLiteral( "Pose" ), globalPlannerModel, QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ), BlockPort::Flag::None );
  obj->addInputPort( QStringLiteral( "Pose Left Edge" ), obj.get(), QLatin1StringView( SLOT( setPoseLeftEdge( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose Right Edge" ), obj.get(), QLatin1StringView( SLOT( setPoseRightEdge( POSE_SIGNATURE ) ) ) );

  obj->addInputPort(
    QStringLiteral( "Field" ), obj.get(), QLatin1StringView( SLOT( setField( std::shared_ptr< Polygon_with_holes_2 > ) ) ) );

  obj->addOutputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SIGNAL( planChanged( const Plan& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Plan Polyline" ),
                      obj.get(),
                      QLatin1StringView( SIGNAL( planPolylineChanged( std::shared_ptr< std::vector< Point_2 > > ) ) ) );

  return obj;
}
