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

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

#include <QtMath>

#include "helpers/eigenHelper.h"
#include "helpers/anglesHelper.h"

#include "VehicleNonLinear3DOF.h"

// VehicleDynamics::VehicleNonLinear3DOF takes ownership of the Tire*s
VehicleDynamics::VehicleNonLinear3DOF::VehicleNonLinear3DOF( VehicleDynamics::Tire* frontTire, VehicleDynamics::Tire* rearTire )
  : frontTire( frontTire ), rearTire( rearTire ) {

  state.setZero();
  oldDState.setZero();

  stateNames
      << QStringLiteral( "X" )
      << QStringLiteral( "Y" )
      << QStringLiteral( "Psi" )
      << QStringLiteral( "V" )
      << QStringLiteral( "AlphaT" )
      << QStringLiteral( "dPsi" );
}

void VehicleDynamics::VehicleNonLinear3DOF::step( double deltaT, double deltaF,
    double fxFrontLeft, double fxFrontRight, double fxRearLeft, double fxRearRight ) {
  double psi = state( 2 );
  double v = state( 3 );
  double alphaT = state( 4 );
  double dPsi = state( 5 );

  double m = mass();
  double a = lengthA();
  double b = lengthB();
  std::cout << "m: " << m << ", a " << a << ", b " << b << ", trackWidth " << trackWidth << std::endl;

  double fzFront = massFrontAxle * G;
  double fzRear  = massRearAxle  * G;
  std::cout << "fz: " << fzFront << ", " << fzRear << std::endl;

  // slip angles
  double alphaFront = std::atan2( v * std::sin( alphaT ) + a * dPsi, v * std::cos( alphaT ) ) - deltaF;
  double alphaRear  = std::atan2( v * std::sin( alphaT ) - b * dPsi, v * std::cos( alphaT ) );

  std::cout << "alpha: " << alphaFront << " (" << radiansToDegrees( alphaFront ) << "), " << alphaRear << " (" << radiansToDegrees( alphaRear ) << ")" << std::endl;
  std::cout << "operands: " << v* std::sin( alphaT ) + a* dPsi << ", " << v* std::cos( alphaT ) << std::endl;


  double fxFront = fxFrontLeft + fxFrontRight;
  double fxRear  = fxRearLeft  + fxRearRight;
  std::cout << "fx: " << fxFront << " " << fxRear << std::endl;

  // tyre characteristics
  double fyFront = 2 * nFrontTiresPerSide * frontTire->characteristic( alphaFront, fzFront / ( 2 * nFrontTiresPerSide ), muy );
  double fyRear  = 2 * nRearTiresPerSide  * rearTire-> characteristic( alphaRear,  fzRear  / ( 2 * nRearTiresPerSide ),  muy );
  std::cout << "fy: " << fyFront << " " << fyRear << std::endl;

  // derived state
  decltype( state ) dState;
  dState( 0 ) = v * std::cos( alphaT + psi );
  dState( 1 ) = v * std::sin( alphaT + psi );
  dState( 2 ) = qIsNull( v ) ? 0 : dPsi;
  dState( 3 ) = ( fxFront * std::cos( alphaT - deltaF ) + fxRear * std::cos( alphaT ) + fyFront * std::sin( alphaT - deltaF ) + fyRear * std::sin( alphaT ) ) / ( m );
  dState( 4 ) = qIsNull( v ) ? 0 : ( - fxFront * std::sin( alphaT - deltaF ) - fxRear * std::sin( alphaT ) + fyFront * std::cos( alphaT - deltaF ) + fyRear * std::cos( alphaT ) - m * v * dPsi ) / ( m * v );
  dState( 5 ) = qIsNull( v ) ? 0 : ( fxFront * a * std::sin( deltaF ) + fyFront * a * std::cos( deltaF ) - fyRear * b ) / momentOfInertia;

  std::cout << "deltaF "/*<<std::endl*/ << deltaF << std::endl;
  std::cout << "deltaT "/*<<std::endl*/ << deltaT << std::endl;
  std::cout << "dState" << std::endl << dState << std::endl;

  // new state, basicaly integrating y'=f(x, t)
  state = state + ( dState * deltaT );

//  state = state + ( oldDState * deltaT ) + (dState-oldDState)*deltaT;
  oldDState = dState;

  state( 2 ) = normalizeAngleRadians( state( 2 ) );
  state( 4 ) = normalizeAngleRadians( state( 4 ) );

  for( int i = 0, end = stateNames.count(); i < end; ++i ) {
    std::cout << std::fixed << std::setw( 12 ) << std::setprecision( 10 ) <</*std::scientific<<*/std::showpos << state( i ) << " " << stateNames[i].toStdString() << std::endl;
  }
}
