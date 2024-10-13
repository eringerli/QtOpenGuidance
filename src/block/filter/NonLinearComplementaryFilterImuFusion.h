// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <memory>

#include "../BlockBase.h"

#include "helpers/eigenHelper.h"

#include <attitude_estimator.h>

namespace stateestimation {
  class AttitudeEstimator;
};

class NonLinearComplementaryFilterImuFusion : public BlockBase {
  Q_OBJECT

public:
  explicit NonLinearComplementaryFilterImuFusion( const int idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setImuData( IMU_SIGNATURE_SLOT );

  void setOrientationCorrection( ORIENTATION_SIGNATURE_SLOT );

  void setKp( NUMBER_SIGNATURE_SLOT );
  void setTi( NUMBER_SIGNATURE_SLOT );

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

  double kP = 2.2;
  double tI = 2.65;

  std::unique_ptr< stateestimation::AttitudeEstimator > est = nullptr;
};

class NonLinearComplementaryFilterImuFusionFactory : public BlockFactory {
  Q_OBJECT

public:
  NonLinearComplementaryFilterImuFusionFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "NLCF for IMU Fusion" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Non-Linear Complementary Filter for IMU Fusion" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Filter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
