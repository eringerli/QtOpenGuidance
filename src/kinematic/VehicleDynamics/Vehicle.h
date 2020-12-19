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

#include <cmath>
#include <memory>

#include "helpers/eigenHelper.h"

#include "Tire.h"

#include <QStringList>

namespace VehicleDynamics {
  class Vehicle {
    public:
      Vehicle() {}
      virtual ~Vehicle() {}

      virtual void step( double deltaT, double deltaF,
                         double fxFrontLeft, double fxFrontRight, double fxRearLeft, double fxRearRight ) = 0;

      /// Mass of the car (tractor) [kg]
      double mass() {
        return massFrontAxle + massRearAxle;
      }

      /// Distance from front axle of the car (tractor) to the center of mass of the car (tractor) [m]
      double lengthA() {
        return massRearAxle / mass() * wheelBase;
      }

      /// Distance from center of mass of the car (tractor) to the front axle of the car (tractor) [m]
      double lengthB() {
        return wheelBase - lengthA();
      }

      QStringList stateNames;

      double momentOfInertia = 10000; /// Moment of inertia the car (tractor) [kg * m2]
      double massFrontAxle = 600; /// Mass over the front axle [kg]
      double massRearAxle = 700; /// Mass over the rear axle [kg]
      double wheelBase = 3.5; /// Wheelbase [m]
      int nFrontTiresPerSide = 1; /// Number of front tires per side
      int nRearTiresPerSide = 1; /// Number of rear tires per side
      double trackWidth = 2; /// Track of the car (tractor)  [m]
      double muy = 0.8; /// Operational friction coefficient

      static constexpr double G = 9.80665;
  };
}
