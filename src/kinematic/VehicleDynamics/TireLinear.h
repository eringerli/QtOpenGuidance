// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://github.com/andresmendes/openvd

#pragma once

#include "Tire.h"

namespace VehicleDynamics {
  class TireLinear : public Tire {
  public:
    TireLinear() {}
    TireLinear( double Cy ) : Cy( Cy ) {}
    virtual ~TireLinear() {}

    virtual double characteristic( double alpha, double, double );

    double Cy = 2400 * 180 / M_PI;
  };
}
