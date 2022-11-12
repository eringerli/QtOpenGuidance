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

#include "FixedKinematic.h"

#include "qneblock.h"
#include "qneport.h"

void
FixedKinematic::setOffsetHookToPivot( const Eigen::Vector3d& offset ) {
  hookToPivot.setOffset( offset );
}

void
FixedKinematic::setOffsetPivotToTow( const Eigen::Vector3d& offset ) {
  pivotToTow.setOffset( offset );
}

void
FixedKinematic::setPose( const Eigen::Vector3d&           position,
                         const Eigen::Quaterniond&        orientation,
                         const CalculationOption::Options options ) {
  hookToPivot.setPose( position, orientation, options );
  pivotToTow.setPose( hookToPivot.positionCalculated, hookToPivot.orientation, options );

  Q_EMIT poseHookPointChanged( position, orientation, options );
  Q_EMIT posePivotPointChanged( hookToPivot.positionCalculated, hookToPivot.orientation, options );
  Q_EMIT poseTowPointChanged( pivotToTow.positionCalculated, pivotToTow.orientation, options );
}

QNEBlock*
FixedKinematicFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FixedKinematic;
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Offset Hook to Pivot" ), QLatin1String( SLOT( setOffsetHookToPivot( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Offset Pivot To Tow" ), QLatin1String( SLOT( setOffsetPivotToTow( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( POSE_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
