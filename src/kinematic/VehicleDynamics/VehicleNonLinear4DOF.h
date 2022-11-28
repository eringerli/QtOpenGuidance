// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

// https://github.com/andresmendes/openvd

#pragma once

#include <memory>

#include "helpers/eigenHelper.h"

#include "Vehicle.h"

namespace VehicleDynamics {
  class VehicleNonLinear4DOF : public Vehicle {
  public:
    VehicleNonLinear4DOF( Tire* frontLeftTire, Tire* frontRightTire, Tire* rearLeftTire, Tire* rearRightTire );
    virtual ~VehicleNonLinear4DOF() {}

    virtual void step( double deltaT, double deltaF, double fxFrontLeft, double fxFrontRight, double fxRearLeft, double fxRearRight );

    Eigen::Matrix< double, 8, 8 > massMatrix();

    enum class StateNames : Eigen::Matrix< double, 8, 8 >::Index{ X, Y, Psi, Theta, V, AlphaT, dPsi, dTheta };

  public:
    QStringList                   stateNames;
    Eigen::Matrix< double, 8, 1 > state;

    Tire* frontLeftTire;
    Tire* frontRightTire;
    Tire* rearLeftTire;
    Tire* rearRightTire;

    //      double fxFrontLeft = 0; /// Longitudinal force at front left tire [N]
    //      double fxFrontRight = 0; /// Longitudinal force at front right tire [N]
    //      double fxRearLeft = 0; /// Longitudinal force at rear left tire [N]
    //      double fxRearRight = 0; /// Longitudinal force at rear right tire [N]
    double K                       = 50000000; /// Torsional stiffness [Nm/rad]
    double C                       = 5000000;  /// Torsional damping [Nms/rad]
    double heightOfCenterOfGravity = 0.2;      /// CG height [m]
    double trackWidth              = 0.6;      /// track width [m]

    // Intertia properties
    double Ixx = 12000;
    double Ixy = 1000;
    double Ixz = 1000;
    double Iyy = 65000;
    double Iyz = 1000;
    double Izz = 65000;
  };
}
