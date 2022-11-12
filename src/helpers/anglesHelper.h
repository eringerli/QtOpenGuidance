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
