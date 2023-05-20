// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

class RateLimiterVector : public BlockBase {
  Q_OBJECT

public:
  explicit RateLimiterVector() : BlockBase() {}
public Q_SLOTS:
  void setRate( NUMBER_SIGNATURE_SLOT );
  void setVector( VECTOR_SIGNATURE_SLOT );

Q_SIGNALS:
  void vectorChanged( VECTOR_SIGNATURE_SLOT );

private:
  double      interval = 1.;
  RateLimiter rateLimiter;
};

class RateLimiterVectorFactory : public BlockFactory {
  Q_OBJECT

public:
  RateLimiterVectorFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "RateLimiterVector" ); }

  QString getPrettyNameOfFactory() override { return QStringLiteral( "RateLimiter for Vector" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Rate Limiter" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};