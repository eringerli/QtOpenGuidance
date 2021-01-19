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

      virtual void step( double deltaT, double deltaF,
                         double fxFront, double fxFrontRight, double fxRearLeft, double fxRearRight );


    public:
      QStringList stateNames;
      Eigen::Matrix<double, 6, 1> state;
      Eigen::Matrix<double, 6, 1> oldDState;

      std::unique_ptr<Tire> frontTire;
      std::unique_ptr<Tire> rearTire;
  };
}
