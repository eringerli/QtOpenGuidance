// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class RateLimiterPose : public BlockBase {
  Q_OBJECT

public:
  explicit RateLimiterPose() : BlockBase() {}
public Q_SLOTS:
  void setRate( NUMBER_SIGNATURE_SLOT );
  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseChanged( POSE_SIGNATURE_SLOT );

private:
  double      interval = 1.;
  RateLimiter rateLimiter;
};

class RateLimiterPoseFactory : public BlockFactory {
  Q_OBJECT

public:
  RateLimiterPoseFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "RateLimiterPose" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "RateLimiter for Pose" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Rate Limiter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
