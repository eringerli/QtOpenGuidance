// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class RateLimiterImu : public BlockBase {
  Q_OBJECT

public:
  explicit RateLimiterImu( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}
public Q_SLOTS:
  void setRate( NUMBER_SIGNATURE_SLOT );
  void setImu( IMU_SIGNATURE_SLOT );

Q_SIGNALS:
  void imuChanged( IMU_SIGNATURE_SLOT );

private:
  RateLimiter rateLimiter;
  double      interval = 1.;
};

class RateLimiterImuFactory : public BlockFactory {
  Q_OBJECT

public:
  RateLimiterImuFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::InputOutput; }

  QString getNameOfFactory() const override { return QStringLiteral( "RateLimiterImu" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "RateLimiter for Imu" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Rate Limiter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
