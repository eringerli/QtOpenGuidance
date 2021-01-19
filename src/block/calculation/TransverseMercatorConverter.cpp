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

#include "TransverseMercatorConverter.h"

#include "qneblock.h"
#include "qneport.h"

void TransverseMercatorConverter::setWGS84Position( const Eigen::Vector3d& position ) {
  double x = 0;
  double y = 0;
  double z = 0;
  tmw->Forward( position.x(), position.y(), position.z(), x, y, z );

  Q_EMIT positionChanged( Eigen::Vector3d( x, y, z ) );
}

void TransverseMercatorConverter::emitConfigSignals() {
  Q_EMIT positionChanged( Eigen::Vector3d( 0, 0, 0 ) );
}

QNEBlock* TransverseMercatorConverterFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new TransverseMercatorConverter( tmw );
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SLOT( setWGS84Position( const Eigen::Vector3d& ) ) ) );

  b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( const Eigen::Vector3d& ) ) ) );

  return b;
}
