// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://github.com/andresmendes/openvd

#pragma once

#include <cmath>
#include <memory>

namespace VehicleDynamics {
  class Tire {
  public:
    Tire() {}
    virtual ~Tire() {}

    virtual double characteristic( double alpha, double Fz, double muy ) = 0;
  };
}
