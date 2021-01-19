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

#include "FixedKinematicPrimitive.h"

#include "qneblock.h"
#include "qneport.h"

void FixedKinematicPrimitive::setOffset( const Eigen::Vector3d& offset ) {
  this->offset = -offset;
}

void FixedKinematicPrimitive::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation, const PoseOption::Options& options ) {
  if( !options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
    orientation = rotation;
  } else {
    orientation = Eigen::Quaterniond();
  }

  Eigen::Vector3d positionCorrection = orientation * offset;

  positionCalculated = Eigen::Vector3d( position.x() + positionCorrection.x(),
                                        position.y() + positionCorrection.y(),
                                        position.z() + positionCorrection.z() );

  Q_EMIT poseChanged( positionCalculated, orientation, options );
}

QNEBlock* FixedKinematicPrimitiveFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FixedKinematicPrimitive;
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Offset" ), QLatin1String( SLOT( setOffset( Eigen::Vector3d ) ) ) );
  b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  return b;
}
