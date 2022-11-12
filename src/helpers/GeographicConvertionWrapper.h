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

#include "eigenHelper.h"

#include <GeographicLib/LocalCartesian.hpp>

// NOTE: QtOpenGuidance uses the coordinate system for the vehicle according to ISO 8855:2011(E)
// (Y left, X forward, Z up), but the coordinate system of the geographic conversions
// is another one: X east, Y north and Z up. As this is the only code that uses both,
// the conversion is done here. This is why the x and y parameters are swapped and y is
// negated.

using namespace std;
using namespace GeographicLib;

// an instance of this class gets shared across all the blocks, so the conversions are the same everywhere
class GeographicConvertionWrapper {
public:
  void Forward( const double latitude, const double longitude, const double height, double& x, double& y, double& z );

  void Forward( const double latitude, const double longitude, double& x, double& y, double& z );

  Eigen::Vector3d Forward( const Eigen::Vector3d& point );

  void Reverse( const double x, const double y, const double z, double& latitude, double& longitude, double& height );

  void Reverse( const double x, const double y, double& latitude, double& longitude, double& height );

  void Reset( const double latitude, const double longitude, const double height );

  Eigen::Vector3d Reverse( const Eigen::Vector3d& point );

public:
  bool useTM = true;

private:
  LocalCartesian _lc;

  bool isLatLonOffsetSet = false;

  double falseNorthingTM = 0;
  double lon0TM          = 0;
  double height0TM       = 0;
};
