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

inline double normalizeAngleRadians( double angle ) {
  while( angle > M_PI ) {
    angle -= M_PI * 2;
  }

  while( angle < -M_PI ) {
    angle += M_PI * 2;
  }

  return angle;
}

inline double normalizeAngleRadians1pi( double angle ) {
  while( angle > M_PI ) {
    angle -= M_PI;
  }

  while( angle < -M_PI ) {
    angle += M_PI;
  }

  return angle;
}

inline double normalizeAngleDegrees( double angle ) {
  while( angle > 180 ) {
    angle -= 360;
  }

  while( angle < -180 ) {
    angle += 360;
  }

  return angle;
}
