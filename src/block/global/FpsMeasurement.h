// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

class FpsMeasurement : public BlockBase {
  Q_OBJECT

public:
  explicit FpsMeasurement( Qt3DCore::QEntity* rootEntity, const BlockBaseId idHint, const bool systemBlock, const QString type );
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
  FpsMeasurementFactory( QThread* thread, Qt3DCore::QEntity* rootEntity ) : BlockFactory( thread, true ), rootEntity( rootEntity ) {
    typeColor = TypeColor::Model;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Fps Measurement" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Graphical" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
};
