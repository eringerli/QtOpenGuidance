// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LocalPlanner.h"

#include <QAction>
#include <QMenu>

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

#include "gui/MyMainWindow.h"
#include "gui/toolbar/GuidanceTurningToolbar.h"

#include "helpers/eigenHelper.h"
#include "kinematic/cgal.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveArc.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include "kinematic/Plan.h"
#include "kinematic/PlanGlobal.h"

#include "block/graphical/PathPlannerModel.h"

#include <dubins/dubins.h>

LocalPlanner::LocalPlanner(
  MyMainWindow* mainWindow, const QString& uniqueName, const int idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  widget = new GuidanceTurningToolbar( mainWindow );
  dock   = new KDDockWidgets::QtWidgets::DockWidget( uniqueName );

  QObject::connect( widget, &GuidanceTurningToolbar::turnLeftToggled, this, &LocalPlanner::turnLeftToggled );
  QObject::connect( widget, &GuidanceTurningToolbar::turnRightToggled, this, &LocalPlanner::turnRightToggled );
  QObject::connect( widget, &GuidanceTurningToolbar::numSkipChanged, this, &LocalPlanner::numSkipChanged );
  QObject::connect( this, &LocalPlanner::resetTurningStateOfDock, widget, &GuidanceTurningToolbar::resetTurningState );
}

LocalPlanner::~LocalPlanner() {
  dock->deleteLater();
  widget->deleteLater();
}

void
LocalPlanner::setName( const QString& name ) {
  BlockBase::setName( name );

  dock->setTitle( name );
  dock->toggleAction()->setText( QStringLiteral( "Turning Dock: " ) + name );

  pathPlannerModel->setName( name + " Model" );
}

void
LocalPlanner::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoPlanner ) ) {
    this->position    = position;
    this->orientation = orientation;

    const Point_2 position2D = to2D( position );

    if( !turningLeft && !turningRight ) {
      if( !globalPlan.plan->empty() ) {
        double distanceSquared          = std::numeric_limits< double >::infinity();
        auto   nearestPrimitive         = globalPlan.getNearestPrimitive( position2D, distanceSquared, &lastNearestPrimitiveGlobalPlan );
        lastNearestPrimitiveGlobalPlan  = nearestPrimitive;
        double distanceNearestPrimitive = std::sqrt( distanceSquared );

        if( !lastPrimitiveGlobalPlan ||
            ( !forceCurrentPath && distanceNearestPrimitive <
                                     ( std::sqrt( lastPrimitiveGlobalPlan->distanceToPointSquared( position2D ) ) - pathHysteresis ) ) ) {
          lastPrimitiveGlobalPlan = *nearestPrimitive;
          localPlan.plan->clear();
        }

        if( lastPrimitiveGlobalPlan->anyDirection ) {
          double angleLastPrimitiveDegrees = lastPrimitiveGlobalPlan->angleAtPointDegrees( position2D );
          double steerAngleAbsoluteDegrees = steeringAngleDegrees + radiansToDegrees( getYaw( quaternionToTaitBryan( orientation ) ) );

          if( std::abs( std::abs( steerAngleAbsoluteDegrees ) - std::abs( angleLastPrimitiveDegrees ) ) > 95 ) {
            auto reverse            = lastPrimitiveGlobalPlan->createReverse();
            reverse->anyDirection   = false;
            lastPrimitiveGlobalPlan = reverse;
            localPlan.plan->clear();
          }
        }

        if( localPlan.plan->empty() ) {
          if( targetPrimitiveGlobalPlan ) {
            localPlan.plan->push_back( targetPrimitiveGlobalPlan );
          } else {
            localPlan.plan->push_back( lastPrimitiveGlobalPlan );
          }

          Q_EMIT planChanged( localPlan );
          Q_EMIT passNumberChanged( double( lastPrimitiveGlobalPlan->passNumber ), CalculationOption::Options() );
        }
      }
    } /* else {
       if( !localPlan.plan->empty() ) {
         const auto* const sequence = localPlan.plan->front()->castToSequence();

         const auto step = sequence->findSequencePrimitiveIndex( position2D );

         if( step >= ( sequence->sequence.size() - 1 ) ) {
           turningLeft  = false;
           turningRight = false;
           Q_EMIT resetTurningStateOfDock();

           setPose( position, orientation, options );
         }
       }
     }*/
  }
}

void
LocalPlanner::setPoseLast( const Eigen::Vector3d&           position,
                           const Eigen::Quaterniond&        orientation,
                           const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoPlanner ) ) {
    if( turningLeft || turningRight ) {
      if( !localPlan.plan->empty() ) {
        const auto* const sequence = localPlan.plan->front()->castToSequence();
        if( sequence ) {
          const Point_2 position2D = to2D( position );

          const auto step = sequence->findSequencePrimitiveIndex( position2D );

          if( step >= ( sequence->sequence.size() - 1 ) ) {
            turningLeft               = false;
            turningRight              = false;
            targetPrimitiveGlobalPlan = nullptr;

            Q_EMIT resetTurningStateOfDock();

            setPose( position, orientation, options );
          }
        }
      }
    }
  }
}

void
LocalPlanner::setPlan( const Plan& plan ) {
  this->globalPlan               = plan;
  lastNearestPrimitiveGlobalPlan = plan.plan->cend();
  lastPrimitiveGlobalPlan        = nullptr;
  targetPrimitiveGlobalPlan      = nullptr;
}

void
LocalPlanner::setSteeringAngle( const double steeringAngle, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    this->steeringAngleDegrees = steeringAngle;
  }
}

void
LocalPlanner::setPathHysteresis( const double pathHysteresis, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    this->pathHysteresis = pathHysteresis;
  }
}

void
LocalPlanner::setMinRadius( const double minRadius, CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoPlanner ) ) {
    this->minRadius = minRadius;
  }
}

void
LocalPlanner::setForceCurrentPath( const bool enabled ) {
  forceCurrentPath = enabled;
}

void
LocalPlanner::turnLeftToggled( bool state ) {
  if( state ) {
    bool existingTurn = turningLeft | turningRight;
    turningLeft       = true;
    turningRight      = false;
    calculateTurning( existingTurn );
  } else {
    turningLeft             = false;
    turningRight            = false;
    lastPrimitiveGlobalPlan = nullptr;
  }
}

void
LocalPlanner::turnRightToggled( bool state ) {
  if( state ) {
    bool existingTurn = turningLeft || turningRight;
    turningRight      = true;
    turningLeft       = false;
    calculateTurning( existingTurn );
  } else {
    turningLeft             = false;
    turningRight            = false;
    lastPrimitiveGlobalPlan = nullptr;
  }
}

void
LocalPlanner::numSkipChanged( int left, int right ) {
  bool recalculate = ( turningLeft && leftSkip != left ) || ( turningRight && rightSkip != right );

  leftSkip  = left;
  rightSkip = right;

  if( recalculate ) {
    calculateTurning( true );
  }
}

void
LocalPlanner::calculateTurning( bool changeExistingTurn ) {
  auto start = std::chrono::high_resolution_clock::now();

  if( !globalPlan.plan->empty() ) {
    const Point_2 position2D     = to2D( position );
    const double  headingDegrees = radiansToDegrees( getYaw( quaternionToTaitBryan( orientation ) ) );

    if( !changeExistingTurn ) {
      positionTurnStart = position2D;
      headingTurnStart  = headingDegrees;
    }

    double distanceSquared  = std::numeric_limits< double >::infinity();
    auto   nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared, &lastNearestPrimitiveGlobalPlan );

    double angleNearestPrimitiveDegrees = ( *nearestPrimitive )->angleAtPointDegrees( positionTurnStart );
    bool   searchUp                     = turningLeft;

    bool reversedLine = std::abs( headingTurnStart - angleNearestPrimitiveDegrees ) > 90;

    if( reversedLine ) {
      searchUp = !searchUp;
    }

    // trigger the calculation of the plan
    double angleOffsetRad = degreesToRadians( angleNearestPrimitiveDegrees );
    auto   offset = polarOffsetRad( angleOffsetRad - M_PI, ( *nearestPrimitive )->implementWidth * ( turningLeft ? leftSkip : rightSkip ) );

    if( turningRight ) {
      offset = -offset;
    }

    if( reversedLine ) {
      offset = -offset;
    }

    Q_EMIT triggerPlanPose( toEigenVector( positionTurnStart + offset ), orientation, CalculationOption::Options() );
    {
      std::cout << "calculateTurning1 "
                << std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::high_resolution_clock::now() - start ).count()
                << "us" << std::endl;
      start = std::chrono::high_resolution_clock::now();
    }
    nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared, &lastNearestPrimitiveGlobalPlan );

    auto perpendicularLine = ( *nearestPrimitive )->perpendicularAtPoint( positionTurnStart );

    auto targetLineIt = globalPlan.plan->cend();

    if( turningLeft ) {
      targetLineIt = nearestPrimitive + ( searchUp ? leftSkip : -leftSkip );
    } else {
      targetLineIt = nearestPrimitive + ( searchUp ? rightSkip : -rightSkip );
    }

    targetPrimitiveGlobalPlan = *targetLineIt;

    if( targetLineIt != globalPlan.plan->cend() ) {
      Point_2 resultingPoint;

      if( ( *targetLineIt )->intersectWithLine( perpendicularLine, resultingPoint ) ) {
        double headingAtTargetRad =
          degreesToRadians( ( *targetLineIt )->angleAtPointDegrees( resultingPoint ) ) + ( reversedLine ? M_PI : 0 );

        DubinsPath dubinsPath;
        double     q0[] = { position2D.x(), position2D.y(), degreesToRadians( headingDegrees ) };
        double     q1[] = { resultingPoint.x(), resultingPoint.y(), headingAtTargetRad - M_PI };
        {
          std::cout << "calculateTurning2 "
                    << std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::high_resolution_clock::now() - start ).count()
                    << "us" << std::endl;
          start = std::chrono::high_resolution_clock::now();
        }
        dubins_shortest_path( &dubinsPath, q0, q1, minRadius );
        {
          std::cout << "calculateTurning3 "
                    << std::chrono::duration_cast< std::chrono::microseconds >( std::chrono::high_resolution_clock::now() - start ).count()
                    << "us" << std::endl;
          start = std::chrono::high_resolution_clock::now();
        }

        auto sequence  = std::vector< std::shared_ptr< PathPrimitive > >();
        auto bisectors = std::vector< Line_2 >();

        Line_2 direction = ( *targetLineIt )->supportingLine( resultingPoint );
        if( !reversedLine ) {
          direction = direction.opposite();
        }

        sequence.push_back( std::make_shared< PathPrimitiveRay >( Ray_2( position2D, direction ), true, 0, false, 0 ) );

        // dubins arcs

        // get the first and last arc
        {
          double qArcPoints[3 * 3];
          dubins_path_sample( &dubinsPath, 0, &qArcPoints[0 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] / 2 ) * dubinsPath.rho, &qArcPoints[1 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] ) * dubinsPath.rho, &qArcPoints[2 * 3] );

          auto arc = std::make_shared< PathPrimitiveArc >( Point_2( qArcPoints[0], qArcPoints[1] ),
                                                           Point_2( qArcPoints[3], qArcPoints[4] ),
                                                           Point_2( qArcPoints[6], qArcPoints[7] ),
                                                           0,
                                                           false,
                                                           0 );

          sequence.push_back( arc );
          bisectors.push_back( arc->startToCenterLine );
          bisectors.push_back( arc->endToCenterLine );
        }

        std::cout << "DubinsPathType: " << dubinsPath.type << std::endl;

        if( dubinsPath.type == DubinsPathType::RLR || dubinsPath.type == DubinsPathType::LRL ) {
          double qArcPoints[5 * 3];
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] ) * dubinsPath.rho, &qArcPoints[0 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] + ( dubinsPath.param[1] / 4 ) ) * dubinsPath.rho, &qArcPoints[1 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] + ( dubinsPath.param[1] / 2 ) ) * dubinsPath.rho, &qArcPoints[2 * 3] );
          dubins_path_sample( &dubinsPath,
                              ( ( dubinsPath.param[0] + dubinsPath.param[1] ) - ( dubinsPath.param[1] / 4 ) ) * dubinsPath.rho,
                              &qArcPoints[3 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] + dubinsPath.param[1] ) * dubinsPath.rho, &qArcPoints[4 * 3] );

          sequence.push_back( std::make_shared< PathPrimitiveArc >( Point_2( qArcPoints[0], qArcPoints[1] ),
                                                                    Point_2( qArcPoints[3], qArcPoints[4] ),
                                                                    Point_2( qArcPoints[6], qArcPoints[7] ),
                                                                    0,
                                                                    false,
                                                                    0 ) );
          bisectors.push_back( sequence.back()->castToArc()->endToCenterLine );

          sequence.push_back( std::make_shared< PathPrimitiveArc >( Point_2( qArcPoints[6], qArcPoints[7] ),
                                                                    Point_2( qArcPoints[9], qArcPoints[10] ),
                                                                    Point_2( qArcPoints[12], qArcPoints[13] ),
                                                                    0,
                                                                    false,
                                                                    0 ) );
        } else {
          double qLinePoints[2 * 3];
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] ) * dubinsPath.rho, &qLinePoints[0 * 3] );
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] + dubinsPath.param[1] ) * dubinsPath.rho, &qLinePoints[1 * 3] );

          sequence.push_back( std::make_shared< PathPrimitiveSegment >(
            Segment_2( Point_2( qLinePoints[0], qLinePoints[1] ), Point_2( qLinePoints[3], qLinePoints[4] ) ), 0, false, 0 ) );
        }

        {
          double qArcPoints[3 * 3];
          dubins_path_sample( &dubinsPath, ( dubinsPath.param[0] + dubinsPath.param[1] ) * dubinsPath.rho, &qArcPoints[0 * 3] );
          dubins_path_sample(
            &dubinsPath, ( dubinsPath.param[0] + dubinsPath.param[1] + ( dubinsPath.param[2] / 2 ) ) * dubinsPath.rho, &qArcPoints[1 * 3] );
          dubins_path_sample(
            &dubinsPath, ( dubinsPath.param[0] + dubinsPath.param[1] + dubinsPath.param[2] ) * dubinsPath.rho, &qArcPoints[2 * 3] );

          auto arc = std::make_shared< PathPrimitiveArc >( Point_2( qArcPoints[0], qArcPoints[1] ),
                                                           Point_2( qArcPoints[3], qArcPoints[4] ),
                                                           Point_2( qArcPoints[6], qArcPoints[7] ),
                                                           0,
                                                           false,
                                                           0 );

          sequence.push_back( arc );
          bisectors.push_back( arc->startToCenterLine );
          bisectors.push_back( arc->endToCenterLine );
        }

        sequence.push_back( std::make_shared< PathPrimitiveRay >( Ray_2( resultingPoint, direction ), false, 0, false, 0 ) );

        std::cout << "Sequence: " << std::endl;
        for( const auto& step : sequence ) {
          step->print();
        }

        localPlan.plan->clear();
        localPlan.plan->push_back( std::make_shared< PathPrimitiveSequence >( sequence, bisectors, 0, false, 0 ) );
        Q_EMIT planChanged( localPlan );
      }
    }
  }
}

std::unique_ptr< BlockBase >
LocalPlannerFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< LocalPlanner >( idHint, mainWindow, getNameOfFactory() + QString::number( idHint ) );

  auto* obj2 = new PathPlannerModel( rootEntity, 0, false, "PathPlannerModel" );
  obj->addAdditionalObject( obj2 );
  obj->pathPlannerModel = obj2;

  QObject::connect( obj.get(), &LocalPlanner::planChanged, obj2, &PathPlannerModel::setPlan );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  mainWindow->addDockWidget( obj->dock, location );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj2, QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ), BlockPort::Flag::None );
  obj->addInputPort( QStringLiteral( "Pose Last" ), obj.get(), QLatin1StringView( SLOT( setPoseLast( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SLOT( setPlan( const Plan& ) ) ) );
  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Path Hysteresis" ), obj.get(), QLatin1StringView( SLOT( setPathHysteresis( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Minimum Radius" ), obj.get(), QLatin1StringView( SLOT( setMinRadius( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Force Current Path" ), obj.get(), QLatin1StringView( SLOT( setForceCurrentPath( ACTION_SIGNATURE ) ) ) );

  obj->addOutputPort( QStringLiteral( "Trigger Plan Pose" ), obj.get(), QLatin1StringView( SIGNAL( triggerPlanPose( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SIGNAL( planChanged( const Plan& ) ) ) );
  obj->addOutputPort( QStringLiteral( "Pass Number" ), obj.get(), QLatin1StringView( SIGNAL( passNumberChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
