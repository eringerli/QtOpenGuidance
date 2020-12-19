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

#include "Vehicle.h"

namespace VehicleDynamics {
  class VehicleNonLinear4DOF : public Vehicle {
    public:
      VehicleNonLinear4DOF( Tire* frontLeftTire, Tire* frontRightTire, Tire* rearLeftTire, Tire* rearRightTire )
        : frontLeftTire( frontLeftTire ), frontRightTire( frontRightTire ), rearLeftTire( rearLeftTire ), rearRightTire( rearRightTire ) {

        state.setZero();

        stateNames
            << QStringLiteral( "X" )
            << QStringLiteral( "Y" )
            << QStringLiteral( "Psi" )
            << QStringLiteral( "Theta" )
            << QStringLiteral( "V" )
            << QStringLiteral( "AlphaT" )
            << QStringLiteral( "dPsi" )
            << QStringLiteral( "dTheta" );
      }
      virtual ~VehicleNonLinear4DOF() {}

      virtual void step( double deltaT, double deltaF,
                         double fxFrontLeft, double fxFrontRight, double fxRearLeft, double fxRearRight );

      Eigen::Matrix<double, 8, 8> massMatrix();

    public:
      QStringList stateNames;
      Eigen::Matrix<double, 8, 1> state;

      std::unique_ptr<Tire> frontLeftTire;
      std::unique_ptr<Tire> frontRightTire;
      std::unique_ptr<Tire> rearLeftTire;
      std::unique_ptr<Tire> rearRightTire;

//      double fxFrontLeft = 0; /// Longitudinal force at front left tire [N]
//      double fxFrontRight = 0; /// Longitudinal force at front right tire [N]
//      double fxRearLeft = 0; /// Longitudinal force at rear left tire [N]
//      double fxRearRight = 0; /// Longitudinal force at rear right tire [N]
      double K = 50000000; /// Torsional stiffness [Nm/rad]
      double C = 5000000; /// Torsional damping [Nms/rad]
      double heightOfCenterOfGravity = 0.2; /// CG height [m]
      double trackWidth = 0.6; /// track width [m]

      // Intertia properties
      double Ixx = 12000;
      double Ixy = 1000;
      double Ixz = 1000;
      double Iyy = 65000;
      double Iyz = 1000;
      double Izz = 65000;
  };
}
