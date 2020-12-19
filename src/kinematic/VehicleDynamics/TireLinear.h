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
#include <iostream>

#include "Tire.h"

namespace VehicleDynamics {
  class TireLinear : public Tire {
    public:
      TireLinear() {}
      TireLinear( double Cy ): Cy( Cy ) {}
      virtual ~TireLinear() {}

      virtual double characteristic( double alpha, double, double ) {
        double ret = - Cy * alpha;

//        if( ret > 5000 ) {
//          ret = 5000;
//        }

//        if( ret < -5000 ) {
//          ret = -5000;
//        }

        std::cout << "Tire " << this << " Alpha: " << alpha << ", Fy: " << ret << std::endl;
        return ret;
      }

      double Cy = 2400 * 180 / M_PI;
  };
}
