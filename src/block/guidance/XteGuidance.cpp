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

void XteGuidance::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond&, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
    const Point_2 position2D = to2D( position );

    if( !plan.plan->empty() ) {
      double distanceSquared = qInf();
      std::shared_ptr<PathPrimitive> nearestPrimitive = nullptr;

      for( const auto& pathPrimitive : *plan.plan ) {
        if( plan.type == Plan::Type::OnlyLines || pathPrimitive->isOn( position2D ) ) {
          double currentDistanceSquared = pathPrimitive->distanceToPointSquared( position2D );

          if( currentDistanceSquared < distanceSquared ) {
            nearestPrimitive = pathPrimitive;
            distanceSquared = currentDistanceSquared;
          } else {
            if( plan.type == Plan::Type::OnlyLines ) {
              // the plan is ordered, so we can take the fast way out...
              break;
            }
          }
        }
      }

      if( nearestPrimitive ) {
        double offsetDistance = std::sqrt( distanceSquared ) * nearestPrimitive->offsetSign( position2D );

        Q_EMIT headingOfPathChanged( nearestPrimitive->angleAtPointDegrees( position2D ) );
        Q_EMIT xteChanged( offsetDistance );
        Q_EMIT passNumberChanged( nearestPrimitive->passNumber );
        return;
      }
    }

    Q_EMIT headingOfPathChanged( qInf() );
    Q_EMIT xteChanged( qInf() );
    Q_EMIT passNumberChanged( qInf() );
  }
}

void XteGuidance::setPlan( const Plan& plan ) {
  this->plan = plan;
}

void XteGuidance::emitConfigSignals() {
  Q_EMIT xteChanged( qInf() );
  Q_EMIT headingOfPathChanged( qInf() );
  Q_EMIT passNumberChanged( qInf() );
}

QNEBlock* XteGuidanceFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new XteGuidance();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
  b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );

  b->addOutputPort( QStringLiteral( "XTE" ), QLatin1String( SIGNAL( xteChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SIGNAL( headingOfPathChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Pass #" ), QLatin1String( SIGNAL( passNumberChanged( const double ) ) ) );

  return b;
}
