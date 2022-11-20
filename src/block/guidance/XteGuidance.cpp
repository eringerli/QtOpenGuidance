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

#include "XteGuidance.h"

#include "qneblock.h"
#include "qneport.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

void
XteGuidance::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::NoXte ) ) {
    const Point_2 position2D = to2D( position );

    double distanceSquared;
    auto   nearestPrimitive = plan.getNearestPrimitive( position2D, distanceSquared, &lastFoundPrimitive );

    lastFoundPrimitive = nearestPrimitive;

    if( nearestPrimitive != plan.plan->cend() ) {
      double offsetDistance = std::sqrt( distanceSquared ) * ( *nearestPrimitive )->offsetSign( position2D );

      Q_EMIT headingOfPathChanged( ( *nearestPrimitive )->angleAtPointDegrees( position2D ), options );
      Q_EMIT curvatureOfPathChanged( ( *nearestPrimitive )->curvature(), options );
      Q_EMIT xteChanged( offsetDistance, options );
      Q_EMIT passNumberChanged( ( *nearestPrimitive )->passNumber, options );
    } else {
      Q_EMIT headingOfPathChanged( std::numeric_limits< double >::infinity(), options );
      Q_EMIT xteChanged( std::numeric_limits< double >::infinity(), options );
      Q_EMIT curvatureOfPathChanged( 0, options );
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
  Q_EMIT xteChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
  Q_EMIT headingOfPathChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
  Q_EMIT passNumberChanged( std::numeric_limits< double >::infinity(), CalculationOption::Option::None );
}

QNEBlock*
XteGuidanceFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new XteGuidance();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );

  b->addOutputPort( QStringLiteral( "XTE" ), QLatin1String( SIGNAL( xteChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SIGNAL( headingOfPathChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Curvature of Path" ), QLatin1String( SIGNAL( curvatureOfPathChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );
  b->addOutputPort( QStringLiteral( "Pass #" ), QLatin1String( SIGNAL( passNumberChanged( NUMBER_SIGNATURE_SIGNAL ) ) ) );

  return b;
}
