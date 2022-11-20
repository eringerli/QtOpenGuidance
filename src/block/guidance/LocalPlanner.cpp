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

#include "LocalPlanner.h"

#include "qneblock.h"
#include "qneport.h"

#include <QAction>
#include <QMenu>

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

#include "gui/GuidanceTurningToolbar.h"
#include "gui/MyMainWindow.h"

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

LocalPlanner::LocalPlanner( const QString& uniqueName, MyMainWindow* mainWindow ) {
  widget = new GuidanceTurningToolbar( mainWindow );
  dock   = new KDDockWidgets::DockWidget( uniqueName );

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
        double distanceSquared          = qInf();
        auto   nearestPrimitive         = globalPlan.getNearestPrimitive( position2D, distanceSquared );
        double distanceNearestPrimitive = std::sqrt( distanceSquared );

        if( !lastPrimitive ||
            ( !forceCurrentPath &&
              distanceNearestPrimitive < ( std::sqrt( lastPrimitive->distanceToPointSquared( position2D ) ) - pathHysteresis ) ) ) {
          lastPrimitive = *nearestPrimitive;
          plan.type     = globalPlan.type;
          plan.plan->clear();
        }

        if( lastPrimitive->anyDirection ) {
          double angleLastPrimitiveDegrees = lastPrimitive->angleAtPointDegrees( position2D );
          double steerAngleAbsoluteDegrees = steeringAngleDegrees + radiansToDegrees( getYaw( quaternionToTaitBryan( orientation ) ) );

          if( std::abs( std::abs( steerAngleAbsoluteDegrees ) - std::abs( angleLastPrimitiveDegrees ) ) > 95 ) {
            auto reverse          = lastPrimitive->createReverse();
            reverse->anyDirection = false;
            lastPrimitive         = reverse;
            plan.type             = globalPlan.type;
            plan.plan->clear();
          }
        }

        if( plan.plan->empty() ) {
          plan.plan->push_back( lastPrimitive );
          Q_EMIT planChanged( plan );
          Q_EMIT passNumberChanged( double( lastPrimitive->passNumber ), CalculationOption::Options() );
        }
      }
    } else {
      if( !plan.plan->empty() ) {
        const auto* const sequence = plan.plan->front()->castToSequence();

        const auto step = sequence->findSequencePrimitiveIndex( position2D );

        //        std::cout << "step: " << step << ", " << sequence->sequence.size() << std::endl;

        if( step >= ( sequence->sequence.size() - 1 ) ) {
          turningLeft  = false;
          turningRight = false;
          Q_EMIT resetTurningStateOfDock();

          setPose( position, orientation, options );
        }
      }
    }
  }
}

void
LocalPlanner::setPlan( const Plan& plan ) {
  this->globalPlan = plan;
  lastPrimitive    = nullptr;
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
    turningLeft   = false;
    turningRight  = false;
    lastPrimitive = nullptr;
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
    turningLeft   = false;
    turningRight  = false;
    lastPrimitive = nullptr;
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

    double distanceSquared  = qInf();
    auto   nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared );

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
    nearestPrimitive = globalPlan.getNearestPrimitive( positionTurnStart, distanceSquared );

    auto perpendicularLine = ( *nearestPrimitive )->perpendicularAtPoint( positionTurnStart );

    auto targetLineIt = globalPlan.plan->cend();

    if( turningLeft ) {
      targetLineIt = nearestPrimitive + ( searchUp ? leftSkip : -leftSkip );
    } else {
      targetLineIt = nearestPrimitive + ( searchUp ? rightSkip : -rightSkip );
    }

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

        plan.plan->clear();
        plan.plan->push_back( std::make_shared< PathPrimitiveSequence >( sequence, bisectors, 0, false, 0 ) );
        plan.type = Plan::Type::Mixed;
        Q_EMIT planChanged( plan );
      }
    }
  }
}

QNEBlock*
LocalPlannerFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new LocalPlanner( getNameOfFactory() + QString::number( id ), mainWindow );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  auto obj2 = new PathPlannerModel( rootEntity );
  b->addObject( obj2 );
  obj->pathPlannerModel = obj2;

  QObject::connect( obj, &LocalPlanner::planChanged, obj2, &PathPlannerModel::setPlan );

  obj->dock->setTitle( getNameOfFactory() );
  obj->dock->setWidget( obj->widget );

  menu->addAction( obj->dock->toggleAction() );

  mainWindow->addDockWidget( obj->dock, location );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Path Hysteresis" ), QLatin1String( SLOT( setPathHysteresis( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Minimum Radius" ), QLatin1String( SLOT( setMinRadius( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Force Current Path" ), QLatin1String( SLOT( setForceCurrentPath( ACTION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Trigger Plan Pose" ), QLatin1String( SIGNAL( triggerPlanPose( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Plan" ), QLatin1String( SIGNAL( planChanged( const Plan& ) ) ) );
  b->addOutputPort( QStringLiteral( "Pass Number" ), QLatin1String( SIGNAL( passNumberChanged( NUMBER_SIGNATURE ) ) ) );

  return b;
}
