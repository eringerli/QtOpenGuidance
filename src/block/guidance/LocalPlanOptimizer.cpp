// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LocalPlanOptimizer.h"

#include "Eigen/src/Core/Matrix.h"
#include "Eigen/src/Geometry/Quaternion.h"

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
#include <memory>

void
LocalPlanOptimizer::setPoseTractor( const Eigen::Vector3d&           position,
                                    const Eigen::Quaterniond&        orientation,
                                    const CalculationOption::Options options ) {
  if( options.testFlag( CalculationOption::CalculateLocalOffsets ) ) {
    this->positionTractor = position;

    auto offset = ( positionTrailer - positionTractor ) / 2.;
    std::cout << "setPoseTractor offset " << offset << std::endl;
    tractorToTrailerHalf.setOffset( offset );
  } else {
    tractorToTrailerHalf.setPose( position, orientation, options );
    auto point2D = to2D( tractorToTrailerHalf.positionCalculated );

    auto primitive = localPlan.plan->front();
    if( auto sequence = primitive->castToSequence() ) {
      qDebug() << "sequence";
      auto entry = sequence->findSequencePrimitive( point2D );

      //      if( auto line = entry->castToLine() ) {
      //        qDebug() << "line";
      //        optimizedPlan.plan->clear();
      //        optimizedPlan.plan->push_back( entry );
      //        Q_EMIT planChanged( optimizedPlan );
      //      }

      //      if( auto ray = entry->castToRay() ) {
      //        qDebug() << "ray";
      //        optimizedPlan.plan->clear();
      //        optimizedPlan.plan->push_back( std::static_pointer_cast< PathPrimitive >( std::make_shared< PathPrimitiveLine >(
      //          entry->supportingLine( point2D ), entry->implementWidth, entry->anyDirection, entry->passNumber ) ) );
      //        Q_EMIT planChanged( optimizedPlan );
      //      }

      //      if( auto segment = entry->castToSegment() ) {
      //        qDebug() << "segment";
      //        optimizedPlan.plan->clear();
      //        optimizedPlan.plan->push_back( std::static_pointer_cast< PathPrimitive >( std::make_shared< PathPrimitiveLine >(
      //          entry->supportingLine( point2D ), entry->implementWidth, entry->anyDirection, entry->passNumber ) ) );
      //        Q_EMIT planChanged( optimizedPlan );
      //      }

      //      if( auto arc = entry->castToArc() ) {
      //        qDebug() << "arc";
      //        optimizedPlan.plan->clear();
      //        optimizedPlan.plan->push_back( std::static_pointer_cast< PathPrimitive >( std::make_shared< PathPrimitiveLine >(
      //          entry->supportingLine( point2D ), entry->implementWidth, entry->anyDirection, entry->passNumber ) ) );
      //        Q_EMIT planChanged( optimizedPlan );
      //      }
    }
  }
}

void
LocalPlanOptimizer::setPoseTrailer( const Eigen::Vector3d&           position,
                                    const Eigen::Quaterniond&        orientation,
                                    const CalculationOption::Options options ) {
  if( options.testFlag( CalculationOption::CalculateLocalOffsets ) ) {
    this->positionTrailer = position;

    auto offset = ( positionTrailer - positionTractor ) / 2.;
    std::cout << "setPoseTrailer offset " << offset << std::endl;
    tractorToTrailerHalf.setOffset( offset );
  }
}

void
LocalPlanOptimizer::setPlan( const Plan& plan ) {
  this->localPlan = plan;

  this->optimizedPlan.plan  = std::make_shared< Plan::PlanData >();
  *this->optimizedPlan.plan = *plan.plan;

  lastNearestPrimitiveLocalPlan = plan.plan->cend();
  lastPrimitiveLocalPlan        = nullptr;
  targetPrimitiveGlobalPlan     = nullptr;

  Q_EMIT triggerPlanPose( Eigen::Vector3d( 0, 0, 0 ), Eigen::Quaterniond( 0, 0, 0, 0 ), CalculationOption::CalculateLocalOffsets );
  Q_EMIT planChanged( optimizedPlan );
}

std::unique_ptr< BlockBase >
LocalPlanOptimizerFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< LocalPlanOptimizer >( idHint, getNameOfFactory() + QString::number( idHint ) );

  auto* obj2 = new PathPlannerModel( rootEntity, 0, false, "PathPlannerModel" );
  obj->addAdditionalObject( obj2 );
  obj->pathPlannerModel = obj2;

  QObject::connect( obj.get(), &LocalPlanOptimizer::planChanged, obj2, &PathPlannerModel::setPlan );

  obj->addInputPort( QStringLiteral( "Pose Tractor" ), obj.get(), QLatin1StringView( SLOT( setPoseTractor( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose Trailer" ), obj.get(), QLatin1StringView( SLOT( setPoseTrailer( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SLOT( setPlan( const Plan& ) ) ) );

  obj->addOutputPort( QStringLiteral( "Trigger Plan Pose" ), obj.get(), QLatin1StringView( SIGNAL( triggerPlanPose( POSE_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SIGNAL( planChanged( const Plan& ) ) ) );

  return obj;
}
