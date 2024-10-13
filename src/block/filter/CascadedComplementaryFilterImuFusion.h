// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QElapsedTimer>
#include <QObject>

#include "../BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class CascadedComplementaryFilterImuFusion : public BlockBase {
  Q_OBJECT

public:
  explicit CascadedComplementaryFilterImuFusion( const int idHint, const bool systemBlock, const QString type );

public Q_SLOTS:
  void setImuData( IMU_SIGNATURE_SLOT );

  void setOrientationCorrection( ORIENTATION_SIGNATURE_SLOT );

private:
  void emitOrientationFromCCF();

Q_SIGNALS:
  void orientationChanged( ORIENTATION_SIGNATURE_SIGNAL );

public:
  virtual void emitConfigSignals() override;

public:
  Eigen::Quaterniond orientation           = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Quaterniond orientationCorrection = Eigen::Quaterniond( 0, 0, 0, 0 );

  double alpha        = 0.8;
  double alphaInverse = 0.2;

  double kP = 25;
  double kI = .1;

  QElapsedTimer elapsedTimer;

private:
  Eigen::Vector3d orientationHat           = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Vector3d orientationErrorIntegral = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Vector3d orientationGyroIntegral  = Eigen::Vector3d( 0, 0, 0 );
};

class CascadedComplementaryFilterImuFusionFactory : public BlockFactory {
  Q_OBJECT

public:
  CascadedComplementaryFilterImuFusionFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "CCF for IMU Fusion" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Cascaded Complementary Filter for IMU Fusion" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Filter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
