// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://github.com/andresmendes/openvd

#pragma once

#include <memory>

#include "helpers/eigenHelper.h"

#include "Vehicle.h"

namespace VehicleDynamics {
  class VehicleNonLinear3DOF : public Vehicle {
  public:
    VehicleNonLinear3DOF( Tire* frontTire, Tire* rearTire );
    virtual ~VehicleNonLinear3DOF() {}

    virtual void step( double deltaT, double deltaF, double fxFront, double fxFrontRight, double fxRearLeft, double fxRearRight );

    enum class StateNames : Eigen::Matrix< double, 6, 1 >::Index{
      X, Y, Psi, V, AlphaT, dPsi,
    };

  public:
    QStringList                   stateNames;
    Eigen::Matrix< double, 6, 1 > state;
    Eigen::Matrix< double, 6, 1 > oldDState;

    Tire* frontTire;
    Tire* rearTire;
  };
}
