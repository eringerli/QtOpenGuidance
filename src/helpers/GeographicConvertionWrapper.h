// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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

  const Eigen::Vector3d Forward( const Eigen::Vector3d& point );

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
