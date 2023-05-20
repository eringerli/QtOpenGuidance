// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class RateLimiterOrientation : public BlockBase {
  Q_OBJECT

public:
  explicit RateLimiterOrientation() : BlockBase() {}
public Q_SLOTS:
  void setRate( NUMBER_SIGNATURE_SLOT );
  void setOrientation( ORIENTATION_SIGNATURE_SLOT );

Q_SIGNALS:
  void orientationChanged( ORIENTATION_SIGNATURE_SLOT );

private:
  double      interval = 1.;
  RateLimiter rateLimiter;
};

class RateLimiterOrientationFactory : public BlockFactory {
  Q_OBJECT

public:
  RateLimiterOrientationFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "RateLimiterOrientation" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "RateLimiter for Orientation" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Rate Limiter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
