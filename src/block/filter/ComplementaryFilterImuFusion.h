// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QObject>

#include "../BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

#include <imu_complementary_filter/complementary_filter.h>

class ComplementaryFilterImuFusion : public BlockBase {
  Q_OBJECT

public:
  explicit ComplementaryFilterImuFusion( const BlockBaseId idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setImuData( IMU_SIGNATURE_SLOT );

  void setOrientationCorrection( ORIENTATION_SIGNATURE_SLOT );

private:
  void emitOrientationFromCCF();

Q_SIGNALS:
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );

  void maxProcessingTimeChanged( NUMBER_SIGNATURE_SIGNAL );
  void processingTimeChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  virtual void emitConfigSignals() override;

public:
  Eigen::Quaterniond orientation           = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Quaterniond orientationCorrection = Eigen::Quaterniond( 0, 0, 0, 0 );

  double alpha        = 0.8;
  double alphaInverse = 0.2;

  double kP = 25;
  double kI = .1;

  std::unique_ptr< imu_tools::ComplementaryFilter > est = nullptr;

private:
};

class ComplementaryFilterImuFusionFactory : public BlockFactory {
  Q_OBJECT

public:
  ComplementaryFilterImuFusionFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "CF for IMU Fusion" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Complementary Filter for IMU Fusion" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Filter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
