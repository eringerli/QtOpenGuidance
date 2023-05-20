// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <memory>

#include "block/BlockBase.h"

class QBasicTimer;

class ValueTransmissionBase : public BlockBase {
  Q_OBJECT

public:
  explicit ValueTransmissionBase( const uint16_t cid );

  QJsonObject toJSON() const override;
  void        fromJSON( QJsonObject& ) override;

public Q_SLOTS:
  void setTimeoutTimeMs( int value );
  void setRepeatTimeMs( int value );

  void setTransmissionId( uint16_t id );

Q_SIGNALS:
  void timedOut( uint16_t id );

protected:
  void         timerEvent( QTimerEvent* event ) override;
  void         resetTimeout();
  virtual void retransmit();

public:
  uint16_t cid           = 0;
  int      timeoutTimeMs = 1000;
  int      repeatTimeMs  = 0;

private:
  std::unique_ptr< QBasicTimer > timeoutTimer;
  std::unique_ptr< QBasicTimer > repeatTimer;
};
