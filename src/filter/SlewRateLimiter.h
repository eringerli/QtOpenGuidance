// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>

#include <QDebug>
#include <limits>

template< typename T >
class SlewRateLimiter {
public:
  explicit SlewRateLimiter( T slewRate ) : slewRate( slewRate ), previousValue( T( 0 ) ) {}

  T set( const T input, const T dT ) {
    if( input != std::numeric_limits< double >::infinity() ) {
      T change = std::clamp( input - previousValue, -slewRate * dT, slewRate * dT );
      previousValue += change;

      return previousValue;
    }
    return 0;
  }

public:
  T previousValue = 0;
  T remainder     = 0;
  T slewRate      = 100;
};
