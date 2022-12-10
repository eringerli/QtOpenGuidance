// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FpsMeasurement.h"

#include "qneblock.h"
#include "qneport.h"

#include <Qt3DCore/QEntity>
#include <Qt3DLogic/QFrameAction>

FpsMeasurement::FpsMeasurement( Qt3DCore::QEntity* rootEntity ) : BlockBase() {
  fpsComponent = new Qt3DLogic::QFrameAction( rootEntity );
  rootEntity->addComponent( fpsComponent );
  QObject::connect( fpsComponent, &Qt3DLogic::QFrameAction::triggered, this, &FpsMeasurement::frameActionTriggered );
}

FpsMeasurement::~FpsMeasurement() { fpsComponent->deleteLater(); }

void
FpsMeasurement::emitConfigSignals() {
  Q_EMIT fpsChanged( 0, CalculationOption::Option::None );
}

void
FpsMeasurement::frameActionTriggered( const float dt ) {
  Q_EMIT fpsChanged( 1 / dt, CalculationOption::Option::None );
}

QNEBlock*
FpsMeasurementFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new FpsMeasurement( rootEntity );
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addOutputPort( QStringLiteral( "FPS" ), QLatin1String( SIGNAL( fpsChanged( NUMBER_SIGNATURE ) ) ) );

  return b;
}
