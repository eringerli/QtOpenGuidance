// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include <QtGlobal>

#include "helpers/signatures.h"
#include "kinematic/CalculationOptions.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>

#include <map>
#include <set>

// mainly from here:
// https://github.com/IBM/sdl2-gamecontroller/blob/main/src/sdlgamecontroller.cpp

#include <QThread>

class SdlInputPollingThread : public QThread {
  Q_OBJECT

public:
  explicit SdlInputPollingThread( QObject* parent ) : QThread( parent ) {}

  void stop() { m_stopped = true; }

Q_SIGNALS:
  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );
  void velocityChanged( NUMBER_SIGNATURE_SIGNAL );

protected:
  // reimplemented from QThread
  virtual void run();

private:
  SDL_GameController* addController( const int device_index );
  void                removeController( const SDL_JoystickID which );

  bool m_stopped = false;

  double velocity   = 0;
  double steerAngle = 0;

  std::map< SDL_JoystickID, SDL_GameController* > gamecontrollers;
};
