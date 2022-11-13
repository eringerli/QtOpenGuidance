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

#include "PoseSynchroniser.h"

#include "qneblock.h"
#include "qneport.h"

void
PoseSynchroniser::setPosition( const Eigen::Vector3d& position ) {
  this->position = position;
  //      QElapsedTimer timer;
  //      timer.start();
  Q_EMIT poseChanged( this->position, orientation, CalculationOption::None );
  //      qDebug() << "Cycle Time PoseSynchroniser:  " << timer.nsecsElapsed() << "ns";
}

void
PoseSynchroniser::setOrientation( const Eigen::Quaterniond& value ) {
  orientation = value;
}

void
PoseSynchroniser::emitConfigSignals() {
  Q_EMIT poseChanged( position, orientation, CalculationOption::None );
}

QNEBlock*
PoseSynchroniserFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new PoseSynchroniser();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Position" ), QLatin1String( SLOT( setPosition( const Eigen::Vector3d& ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( const Eigen::Quaterniond& ) ) ) );

  b->addOutputPort( QStringLiteral( "Pose" ), QLatin1String( SIGNAL( poseChanged( POSE_SIGNATURE ) ) ) );

  return b;
}
