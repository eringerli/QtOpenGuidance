// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <chrono>

class RateLimiter {
public:
  enum class Type : int { PlotDock, ValueDock, Graphical, End };

  inline bool expired( const Type type ) {
    std::chrono::duration< double > duration = std::chrono::steady_clock::now() - lastEvent;

    if( duration.count() > intervals[( int )type] ) {
      lastEvent = std::chrono::steady_clock::now();
      return true;
    }

    return false;
  }

  inline bool expired( const double interval ) {
    std::chrono::duration< double > duration = std::chrono::steady_clock::now() - lastEvent;

    if( duration.count() > interval ) {
      lastEvent = std::chrono::steady_clock::now();
      return true;
    }

    return false;
  }

  static void setRate( const Type type, const double rate ) {
    if( rate > 0.01 ) {
      intervals[( int )type] = 1. / rate;
    }
  }

private:
  std::chrono::time_point< std::chrono::steady_clock > lastEvent = std::chrono::steady_clock::now();

  static double intervals[( int )Type::End];
};
