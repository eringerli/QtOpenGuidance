// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

class FpsMeasurement : public BlockBase {
  Q_OBJECT

public:
  explicit FpsMeasurement( Qt3DCore::QEntity* rootEntity );
  ~FpsMeasurement();

  void emitConfigSignals() override;

public Q_SLOTS:
  void frameActionTriggered( const float dt );

Q_SIGNALS:
  void fpsChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  Qt3DLogic::QFrameAction* fpsComponent;
};

class FpsMeasurementFactory : public BlockFactory {
  Q_OBJECT

public:
  FpsMeasurementFactory( QThread* thread, Qt3DCore::QEntity* rootEntity ) : BlockFactory( thread ), rootEntity( rootEntity ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Fps Measurement" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
};
