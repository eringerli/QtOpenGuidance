// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://github.com/andresmendes/openvd

#include "TireLinear.h"

#include <iostream>

double
VehicleDynamics::TireLinear::characteristic( double alpha, double, double ) {
  double ret = -Cy * alpha;

  //        if( ret > 5000 ) {
  //          ret = 5000;
  //        }

  //        if( ret < -5000 ) {
  //          ret = -5000;
  //        }

  std::cout << "Tire " << this << " Alpha: " << alpha << ", Fy: " << ret << std::endl;
  return ret;
}
