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

#include "helpers/anglesHelper.h"
#include "helpers/eigenHelper.h"

#include "VehicleNonLinear4DOF.h"

VehicleDynamics::VehicleNonLinear4DOF::VehicleNonLinear4DOF( VehicleDynamics::Tire* frontLeftTire,
                                                             VehicleDynamics::Tire* frontRightTire,
                                                             VehicleDynamics::Tire* rearLeftTire,
                                                             VehicleDynamics::Tire* rearRightTire )
    : frontLeftTire( frontLeftTire ), frontRightTire( frontRightTire ), rearLeftTire( rearLeftTire ), rearRightTire( rearRightTire ) {
  state.setZero();

  stateNames << QStringLiteral( "X" ) << QStringLiteral( "Y" ) << QStringLiteral( "Psi" ) << QStringLiteral( "Theta" )
             << QStringLiteral( "V" ) << QStringLiteral( "AlphaT" ) << QStringLiteral( "dPsi" ) << QStringLiteral( "dTheta" );
}

void
VehicleDynamics::VehicleNonLinear4DOF::step(
  double deltaT, double deltaF, double fxFrontLeft, double fxFrontRight, double fxRearLeft, double fxRearRight ) {
  std::cout << std::endl << "forces " << fxFrontLeft << " " << fxFrontRight << " " << fxRearLeft << " " << fxRearRight << std::endl;

  double psi    = state( 2 );
  double theta  = state( 3 );
  double v      = state( 4 );
  double alphaT = state( 5 );
  double dPsi   = state( 6 );
  double dTheta = state( 7 );

  double m = mass();
  double a = lengthA();
  double b = lengthB();
  std::cout << "m: " << m << ", a " << a << ", b " << b << ", trackWidth " << trackWidth << std::endl;

  // weight forces
  double fzRight = ( m * G * trackWidth / 2 + K * theta + C * dTheta ) / trackWidth;
  double fzLeft  = -( -m * G * trackWidth / 2 + K * theta + C * dTheta ) / trackWidth;

  double fzFrontRight = fzRight * b / ( a + b );
  double fzFrontLeft  = fzLeft * b / ( a + b );
  double fzRearRight  = fzRight * a / ( a + b );
  double fzRearLeft   = fzLeft * a / ( a + b );
  std::cout << "fz: " << fzFrontLeft << ", " << fzFrontRight << ", " << fzRearLeft << ", " << fzRearRight << std::endl;

  // slip angles
  double alphaFrontLeft  = std::atan2( ( v * std::sin( alphaT ) + dPsi * a ), ( v * std::cos( alphaT ) - dPsi * trackWidth / 2 ) ) - deltaF;
  double alphaFrontRight = std::atan2( ( v * std::sin( alphaT ) + dPsi * a ), ( v * std::cos( alphaT ) + dPsi * trackWidth / 2 ) ) - deltaF;

  double alphaRearLeft  = std::atan2( ( v * std::sin( alphaT ) - dPsi * b ), ( v * std::cos( alphaT ) - dPsi * trackWidth / 2 ) );
  double alphaRearRight = std::atan2( ( v * std::sin( alphaT ) - dPsi * b ), ( v * std::cos( alphaT ) + dPsi * trackWidth / 2 ) );

  std::cout << "alpha: " << alphaFrontLeft << ", " << alphaFrontRight << ", " << alphaRearLeft << ", " << alphaRearRight << std::endl;

  // tyre characteristics
  double fyFrontLeft  = nFrontTiresPerSide * frontLeftTire->characteristic( alphaFrontLeft, fzFrontLeft / nFrontTiresPerSide, muy );
  double fyFrontRight = nFrontTiresPerSide * frontRightTire->characteristic( alphaFrontRight, fzFrontRight / nFrontTiresPerSide, muy );
  double fyRearLeft   = nRearTiresPerSide * rearLeftTire->characteristic( alphaRearLeft, fzRearLeft / nRearTiresPerSide, muy );
  double fyRearRight  = nRearTiresPerSide * rearRightTire->characteristic( alphaRearRight, fzRearRight / nRearTiresPerSide, muy );

  // derived state
  Eigen::Matrix< double, 8, 1 > dState;
  dState( 0 ) = v * cos( alphaT + psi );
  dState( 1 ) = v * sin( alphaT + psi );
  dState( 2 ) = dPsi;
  dState( 3 ) = dTheta;

  dState( 4 ) = fxRearLeft + fxRearRight - sin( deltaF ) * ( fyFrontLeft + fyFrontRight ) + cos( deltaF ) * ( fxFrontLeft + fxFrontRight ) +
                dPsi * m * ( v * sin( alphaT ) - 2 * dTheta * heightOfCenterOfGravity * cos( theta ) );

  dState( 5 ) =
    fyRearLeft + fyRearRight + sin( deltaF ) * ( fxFrontLeft + fxFrontRight ) -
    m * ( heightOfCenterOfGravity * sin( theta ) * ( std::pow( dTheta, 2 ) + std::pow( dPsi, 2 ) ) + dPsi * v * cos( alphaT ) ) +
    cos( deltaF ) * ( fyFrontLeft + fyFrontRight );

  dState( 6 ) = Iyz * std::pow( dPsi, 2 ) - C * dTheta - K * theta - 2 * Iyz * std::pow( dPsi, 2 ) * std::pow( cos( theta ), 2 ) +
                ( Iyy * std::pow( dPsi, 2 ) * sin( 2 * theta ) ) / 2 - ( Izz * std::pow( dPsi, 2 ) * sin( 2 * theta ) ) / 2 +
                G * heightOfCenterOfGravity * m * sin( theta ) + dPsi * heightOfCenterOfGravity * m * v * cos( alphaT ) * cos( theta );

  dState( 7 ) = Ixy * std::pow( dTheta, 2 ) - ( Ixz * std::pow( dPsi, 2 ) * sin( 2 * theta ) ) / 2 -
                Ixy * std::pow( dPsi, 2 ) * std::pow( sin( theta ), 2 ) - fyRearLeft * b * cos( theta ) - fyRearRight * b * cos( theta ) -
                ( fxRearLeft * trackWidth * cos( theta ) ) / 2 + ( fxRearRight * trackWidth * cos( theta ) ) / 2 +
                2 * Iyz * dTheta * dPsi * cos( theta ) + Ixx * dTheta * dPsi * sin( theta ) - Iyy * dTheta * dPsi * sin( theta ) +
                Izz * dTheta * dPsi * sin( theta ) + fyFrontLeft * a * cos( deltaF ) * cos( theta ) +
                fyFrontRight * a * cos( deltaF ) * cos( theta ) - ( fxFrontLeft * trackWidth * cos( deltaF ) * cos( theta ) ) / 2 +
                ( fxFrontRight * trackWidth * cos( deltaF ) * cos( theta ) ) / 2 + fxFrontLeft * a * cos( theta ) * sin( deltaF ) +
                fxFrontRight * a * cos( theta ) * sin( deltaF ) + ( fyFrontLeft * trackWidth * cos( theta ) * sin( deltaF ) ) / 2 -
                ( fyFrontRight * trackWidth * cos( theta ) * sin( deltaF ) ) / 2;

  auto M = massMatrix();

  //  std::cout << "MassMatrix"<<std::endl<<M<<std::endl;

  Eigen::FullPivLU< decltype( massMatrix() ) > lu( M );
  auto                                         Minv = lu.inverse();
  //  std::cout << "MassMatrix inverse "<<bool(lu.isInvertible())
  //  <<std::endl<<Minv<<std::endl;
  std::cout << "deltaF " /*<<std::endl*/ << deltaF << std::endl;
  std::cout << "deltaT " /*<<std::endl*/ << deltaT << std::endl;
  std::cout << "dState" << std::endl << dState << std::endl;

  dState = Minv * dState;
  std::cout << "Minv * dState" << std::endl << dState << std::endl;

  // new state, basicaly y'=Minv * f(x, t)
  dState     = dState * deltaT;
  state( 0 ) = dState( 0 );
  state( 1 ) = dState( 1 );
  state( 2 ) = normalizeAngleRadians( dState( 2 ) );
  state( 3 ) = dState( 3 );
  state( 4 ) = dState( 4 );
  state( 5 ) = normalizeAngleRadians( dState( 5 ) );
  state( 6 ) = dState( 6 );
  state( 7 ) = dState( 7 );

  for( int i = 0, end = stateNames.count(); i < end; ++i ) {
    std::cout << std::fixed << std::setw( 12 ) << std::setprecision( 10 ) << /*std::scientific<<*/ std::showpos << state( i ) << " "
              << stateNames[i].toStdString() << std::endl;
  }
}

Eigen::Matrix< double, 8, 8 >
VehicleDynamics::VehicleNonLinear4DOF::massMatrix() {
  double m      = mass();
  double theta  = state( 3 );
  double v      = state( 4 );
  double alphaT = state( 5 );

  Eigen::Matrix< double, 8, 8 > M;
  M << 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, m * cos( alphaT ),
    -m * v * sin( alphaT ), heightOfCenterOfGravity * m * sin( theta ), 0, 0, 0, 0, 0, m * sin( alphaT ), m * v * cos( alphaT ), 0,
    -heightOfCenterOfGravity * m * cos( theta ), 0, 0, 0, 0, -heightOfCenterOfGravity * m * cos( theta ) * sin( alphaT ),
    -heightOfCenterOfGravity * m * v * cos( alphaT ) * cos( theta ), -Ixz * cos( theta ) - Ixy * sin( theta ), Ixx, 0, 0, 0, 0, 0, 0,
    Izz * cos( theta ) - Iyz * sin( theta ), -Ixz;

  return M;
}
