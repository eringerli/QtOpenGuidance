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
  explicit RateLimiterOrientation( const BlockBaseId idHint, const bool systemBlock, const QString type )
      : BlockBase( idHint, systemBlock, type ) {}
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
  RateLimiterOrientationFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::InputOutput; }

  QString getNameOfFactory() const override { return QStringLiteral( "RateLimiterOrientation" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "RateLimiter for Orientation" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Rate Limiter" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
