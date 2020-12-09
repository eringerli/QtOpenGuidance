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

inline double normalizeAngleRadians( const double angle ) {
  auto angleToConvert = angle;

  while( angleToConvert > M_PI ) {
    angleToConvert -= M_PI * 2;
  }

  while( angleToConvert < -M_PI ) {
    angleToConvert += M_PI * 2;
  }

  return angleToConvert;
}

inline double normalizeAngleRadians1pi( const double angle ) {
  auto angleToConvert = angle;

  while( angleToConvert > M_PI ) {
    angleToConvert -= M_PI;
  }

  while( angleToConvert < -M_PI ) {
    angleToConvert += M_PI;
  }

  return angleToConvert;
}

inline double normalizeAngleDegrees( const double angle ) {
  auto angleToConvert = angle;

  while( angleToConvert > 180 ) {
    angleToConvert -= 360;
  }

  while( angleToConvert < -180 ) {
    angleToConvert += 360;
  }

  return angleToConvert;
}
