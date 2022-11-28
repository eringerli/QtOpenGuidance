// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>

template< class T >
constexpr T
normalizeAngleRadians( const T angle ) {
  return std::remainder( angle + T( M_PI ) * 2, T( M_PI ) * 2 );
}

template< class T >
constexpr T
normalizeAngleRadians1pi( const T angle ) {
  return std::remainder( angle + T( M_PI ), T( M_PI ) );
}

template< class T >
constexpr T
normalizeAngleDegrees( const T angle ) {
  return std::remainder( angle + T( 360. ), T( 360. ) );
}

template< class T >
constexpr T
degreesToRadians( const T degrees ) {
  return degrees * ( M_PI / 180. );
}

template< class T >
constexpr T
radiansToDegrees( const T radians ) {
  return radians * ( 180. / M_PI );
}
