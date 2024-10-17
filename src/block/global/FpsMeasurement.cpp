// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FpsMeasurement.h"

#include <Qt3DCore/QEntity>
#include <Qt3DLogic/QFrameAction>

FpsMeasurement::FpsMeasurement( Qt3DCore::QEntity*         rootEntity,
                                const BlockBaseId          idHint,
                                const bool                 systemBlock,
                                const QString              type,
                                const BlockBase::TypeColor typeColor )
    : BlockBase( idHint, systemBlock, type, typeColor ) {
  fpsComponent = new Qt3DLogic::QFrameAction( rootEntity );
  rootEntity->addComponent( fpsComponent );
  QObject::connect( fpsComponent, &Qt3DLogic::QFrameAction::triggered, this, &FpsMeasurement::frameActionTriggered );
}

FpsMeasurement::~FpsMeasurement() { fpsComponent->deleteLater(); }

void
FpsMeasurement::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT fpsChanged( 0, CalculationOption::Option::None );
}

void
FpsMeasurement::frameActionTriggered( const float dt ) {
  Q_EMIT fpsChanged( 1 / dt, CalculationOption::Option::None );
}

std::unique_ptr< BlockBase >
FpsMeasurementFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< FpsMeasurement >( idHint, rootEntity );

  obj->addOutputPort( QStringLiteral( "FPS" ), obj.get(), QLatin1StringView( SIGNAL( fpsChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
