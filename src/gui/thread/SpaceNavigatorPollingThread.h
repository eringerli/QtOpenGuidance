// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QtGlobal>

#include "helpers/signatures.h"
#include "kinematic/CalculationOptions.h"

// do not include the config of SPNAV, so X11 doesn't get pulled in...
#define SPNAV_CONFIG_H_

#include <spnav.h>

// mainly from here:
// https://github.com/KDE/calligra/tree/master/plugins/spacenavigator

#include <QThread>

class SpaceNavigatorPollingThread : public QThread {
  Q_OBJECT

public:
  explicit SpaceNavigatorPollingThread( QObject* parent ) : QThread( parent ) {}

  void stop() { m_stopped = true; }

Q_SIGNALS:
  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

protected:
  // reimplemented from QThread
  virtual void run();

private:
  bool m_stopped = false;
};
