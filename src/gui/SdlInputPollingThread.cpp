// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "SdlInputPollingThread.h"

#include <QDebug>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>

#include <chrono>
#include <cmath>
#include <set>
#include <string>

void
SdlInputPollingThread::run() {
  m_stopped = false;

  if( SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) < 0 ) {
    qDebug() << "SDL2 initialization failed!!!";
  } else {
    SDL_version linked;

    SDL_GetVersion( &linked );

    qDebug() << "SDL2 initialization with version " << linked.major << linked.minor << linked.patch;
  }

  for( auto i = 0; i < SDL_NumJoysticks(); ++i ) {
    if( SDL_IsGameController( i ) ) {
      auto gc = addController( i );
      if( gc ) {
        qDebug() << "success adding Controller" << i;
      } else {
        qDebug() << "adding Controller" << i << "failed!!!";
      }
    }
  }

  //  if( spnav_open() == -1 ) {
  //    return;
  //  }

  while( !m_stopped ) {
    auto start = std::chrono::system_clock::now();

    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
      // only collect events for a "while". If we take too long just quit.
      auto now     = std::chrono::system_clock::now();
      auto msSofar = std::chrono::duration_cast< std::chrono::milliseconds >( now - start ).count();
      if( msSofar > 100 ) {
        qDebug() << "Timeout!!!";
        break;
      }

      switch( event.type ) {
        case SDL_CONTROLLERDEVICEADDED: {
          SDL_GameController* gc = addController( event.cdevice.which );
          if( gc ) {
            qDebug() << "success adding Controller" << event.cdevice.which;
          } else {
            qDebug() << "adding Controller" << event.cdevice.which << "failed!!!";
          }
        } break;
        case SDL_CONTROLLERDEVICEREMOVED: {
          removeController( static_cast< int >( event.cdevice.which ) );
        } break;

        case SDL_CONTROLLERAXISMOTION: {
          std::string gcBtn = SDL_GameControllerGetStringForAxis( static_cast< SDL_GameControllerAxis >( event.jaxis.axis ) );

          if( gcBtn == "leftx" ) {
            double newSteerAngle =
              double( event.jaxis.value ) * ( -45. / double( std::numeric_limits< typeof( event.jaxis.value ) >::max() ) );

            if( std::abs( newSteerAngle ) < 5. ) {
              if( std::abs( steerAngle ) > 0.1 ) {
                Q_EMIT steerAngleChanged( 0., CalculationOption::Option::None );
              }
              steerAngle = 0;
            } else {
              steerAngle = newSteerAngle - std::copysign( 5, newSteerAngle );
              Q_EMIT steerAngleChanged( steerAngle, CalculationOption::Option::None );
            }
          }

          if( gcBtn == "lefty" ) {
            double newVelocity =
              double( event.jaxis.value ) * ( -15. / double( std::numeric_limits< typeof( event.jaxis.value ) >::max() ) );

            if( std::abs( newVelocity ) < 5. ) {
              if( std::abs( velocity ) > 0.1 ) {
                Q_EMIT velocityChanged( 0., CalculationOption::Option::None );
              }
              velocity = 0;
            } else {
              velocity = newVelocity - std::copysign( 5, newVelocity );
              Q_EMIT velocityChanged( velocity, CalculationOption::Option::None );
            }
          }
          //        }

          //          qDebug() << "Axis" << gcBtn.c_str() << event.jaxis.value;
          //          obj.Set( "button", gcBtn );
          //          obj.Set( "timestamp", event.jaxis.timestamp );
          //          obj.Set( "value", event.jaxis.value );

          //#if SDL_VERSION_ATLEAST( 2, 0, 12 )
          //          gc = gamecontrollers[event.jaxis.which];
          //          if( gc ) {
          //            auto player = SDL_GameControllerGetPlayerIndex( gc );
          //            obj.Set( "player", player );
          //          }
          //#endif

          //          emit( { Napi::String::New( env, gcBtn ), obj } );
          //          emit( { Napi::String::New( env, "controller-axis-motion" ), obj } );
        } break;

          //        case SDL_CONTROLLERBUTTONDOWN: {
          //          //          obj.Set( "message", "Game controller button pressed" );
          //          std::string gcBtn =
          //            SDL_GameControllerGetStringForButton( static_cast< SDL_GameControllerButton >( event.cbutton.button ) );
          //          qDebug() << "Button" << gcBtn.c_str() << "pressed";
          //          //          obj.Set( "button", gcBtn );
          //          //          obj.Set( "pressed", true );

          //          //#if SDL_VERSION_ATLEAST( 2, 0, 12 )
          //          //          gc = gamecontrollers[event.cbutton.which];
          //          //          if( gc ) {
          //          //            auto player = SDL_GameControllerGetPlayerIndex( gc );
          //          //            obj.Set( "player", player );
          //          //          }
          //          //#endif

          //          //          emit( { Napi::String::New( env, gcBtn + ":down" ), obj } );
          //          //          emit( { Napi::String::New( env, gcBtn ), obj } );
          //          //          emit( { Napi::String::New( env, "controller-button-down" ), obj } );
          //        } break;
          //        case SDL_CONTROLLERBUTTONUP: {
          //          //          obj.Set( "message", "Game controller button released" );
          //          std::string gcBtn =
          //            SDL_GameControllerGetStringForButton( static_cast< SDL_GameControllerButton >( event.cbutton.button ) );
          //          qDebug() << "Button" << gcBtn.c_str() << "released";
          //          //          obj.Set( "button", gcBtn );
          //          //          obj.Set( "pressed", false );

          //          //#if SDL_VERSION_ATLEAST( 2, 0, 12 )
          //          //          gc = gamecontrollers[event.cbutton.which];
          //          //          if( gc ) {
          //          //            auto player = SDL_GameControllerGetPlayerIndex( gc );
          //          //            obj.Set( "player", player );
          //          //          }
          //          //#endif

          //          //          emit( { Napi::String::New( env, gcBtn + ":up" ), obj } );
          //          //          emit( { Napi::String::New( env, gcBtn ), obj } );
          //          //          emit( { Napi::String::New( env, "controller-button-up" ), obj } );
          //        } break;

          //        case SDL_CONTROLLERDEVICEREMAPPED: {
          //          //          obj.Set( "message", "The controller mapping was updated" );
          //          //          obj.Set( "which", static_cast< int >( event.cdevice.which ) );
          //          //          emit( { Napi::String::New( env, "controller-device-remapped" ), obj } );
          //        } break;
      }
    }
    msleep( 10 );
  }
}

SDL_GameController*
SdlInputPollingThread::addController( const int device_index ) {
  SDL_JoystickID controller_id = SDL_JoystickGetDeviceInstanceID( device_index );
  if( controller_id < 0 ) {
    SDL_Log( "Couldn't get controller ID: %s\n", SDL_GetError() );
    return nullptr;
  }

  auto controller = gamecontrollers[controller_id];
  if( controller ) {
    // We already have this controller
    return controller;
  }

  // remember this controller
  controller = SDL_GameControllerOpen( device_index );
  if( controller ) {
    gamecontrollers[controller_id] = controller;
  } else {
    return nullptr;
  }

  SDL_ClearError();

  return controller;
}

void
SdlInputPollingThread::removeController( const SDL_JoystickID which ) {
  auto search = gamecontrollers.find( which );
  if( search != gamecontrollers.end() ) {
    gamecontrollers.erase( search );
  }
}
