// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XteGuidance.h"

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

void
XteGuidance::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoXte ) ) {
    const Point_2 position2D = to2D( position );

    double distanceSquared;
    auto   nearestPrimitive = plan.getNearestPrimitive( position2D, distanceSquared, &lastFoundPrimitive );

    lastFoundPrimitive = nearestPrimitive;

    if( nearestPrimitive != plan.plan->cend() ) {
      double offsetDistance  = std::sqrt( distanceSquared ) * ( *nearestPrimitive )->offsetSign( position2D );
      auto   taitBryanAngles = quaternionToTaitBryan( orientation );

      double headingOfPathDegrees = ( *nearestPrimitive )->angleAtPointDegrees( position2D );

      Q_EMIT headingOfPathChanged( headingOfPathDegrees, options );

      Q_EMIT headingDifferenceChanged( normalizeAngleDegrees( headingOfPathDegrees - radiansToDegrees( getYaw( taitBryanAngles ) ) ),
                                       CalculationOption::Option::None );

      Q_EMIT curvatureOfPathChanged( ( *nearestPrimitive )->curvature(), options );
      Q_EMIT xteChanged( offsetDistance, options );
      Q_EMIT passNumberChanged( ( *nearestPrimitive )->passNumber, options );
    } else {
      Q_EMIT headingOfPathChanged( std::numeric_limits< double >::infinity(), options );
      Q_EMIT headingDifferenceChanged( 0, CalculationOption::Option::None );
      Q_EMIT xteChanged( std::numeric_limits< double >::infinity(), options );
      Q_EMIT curvatureOfPathChanged( std::numeric_limits< double >::infinity(), options );
      Q_EMIT passNumberChanged( std::numeric_limits< double >::infinity(), options );
    }
  }
}

void
XteGuidance::setPlan( const Plan& plan ) {
  this->plan         = plan;
  lastFoundPrimitive = plan.plan->cend();
}

void
XteGuidance::emitConfigSignals() {
  BlockBase::emitConfigSignals();

  Q_EMIT xteChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
  Q_EMIT headingOfPathChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
  Q_EMIT headingDifferenceChanged( 0, CalculationOption::Option::None );
  Q_EMIT passNumberChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
XteGuidanceFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< XteGuidance >( idHint );

  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Plan" ), obj.get(), QLatin1StringView( SLOT( setPlan( const Plan& ) ) ) );

  obj->addOutputPort( QStringLiteral( "XTE" ), obj.get(), QLatin1StringView( SIGNAL( xteChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Heading of Path" ), obj.get(), QLatin1StringView( SIGNAL( headingOfPathChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Heading Difference" ), obj.get(), QLatin1StringView( SIGNAL( headingDifferenceChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Curvature of Path" ), obj.get(), QLatin1StringView( SIGNAL( curvatureOfPathChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  obj->addOutputPort( QStringLiteral( "Pass #" ), obj.get(), QLatin1StringView( SIGNAL( passNumberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return obj;
}
