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

#include "FpsMeasurement.h"

#include "qneblock.h"
#include "qneport.h"

#include <Qt3DLogic/QFrameAction>
#include <Qt3DCore/QEntity>

FpsMeasurement::FpsMeasurement( Qt3DCore::QEntity* rootEntity )
  : BlockBase() {
  fpsComponent = new Qt3DLogic::QFrameAction( rootEntity );
  rootEntity->addComponent( fpsComponent );
  QObject::connect( fpsComponent, &Qt3DLogic::QFrameAction::triggered, this, &FpsMeasurement::frameActionTriggered );
}

FpsMeasurement::~FpsMeasurement() {
  fpsComponent->deleteLater();
}

void FpsMeasurement::emitConfigSignals() {
  Q_EMIT fpsChanged( 0 );
}

void FpsMeasurement::frameActionTriggered( const float dt ) {
  Q_EMIT fpsChanged( 1 / dt );
}

QNEBlock* FpsMeasurementFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FpsMeasurement( rootEntity );
  auto* b = createBaseBlock( scene, obj, id, true );

  b->addOutputPort( QStringLiteral( "FPS" ), QLatin1String( SIGNAL( fpsChanged( double ) ) ) );

  return b;
}
