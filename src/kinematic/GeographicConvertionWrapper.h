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

#pragma once

#include <GeographicLib/TransverseMercator.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <GeographicLib/Ellipsoid.hpp>

// NOTE: QtOpenGuidance uses the coordinate system for the vehicle according to ISO 8855:2011(E)
// (Y left, X forward, Z up), but the coordinate system of the geographic conversions
// is another one: X east, Y north and Z up. As this is the only code that uses both,
// the conversion is done here. This is why the x and y parameters are swapped and y is
// negated.

using namespace std;
using namespace GeographicLib;

// an instance of this class gets shared across all the blocks, so the conversions are the same everywhere
class GeographicConvertionWrapper {
  public:

    GeographicConvertionWrapper() {
    }

    void Forward( const double latitude, const double longitude, const double height, double& x, double& y, double& z ) {
      if( !isLatLonOffsetSet ) {
        Reset( latitude, longitude, height );

        x = 0;
        y = 0;
        z = 0;

        return;
      }

      if( qIsNull( height0TM ) ) {
        height0TM = height;
      }

      if( useTM ) {
        TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, y, x );
        x -= falseNorthingTM;
        y = -y;
        z = height - height0TM;
      } else {
        _lc.Forward( latitude, longitude,  height, y, x, z );
        y = -y;
      }

    }

    void Forward( const double latitude, const double longitude, double& x, double& y, double& z ) {
      if( !isLatLonOffsetSet ) {
        Reset( latitude, longitude, height0TM );

        x = 0;
        y = 0;
        z = 0;

        return;
      }

      if( useTM ) {
        TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, y, x );
        x -= falseNorthingTM;
        y = -y;
        z = 0;
      } else {
        _lc.Forward( latitude, longitude, _lc.HeightOrigin(), y, x, z );
        y = -y;
      }
    }

    void Reverse( const double x, const double y, const double z, double& latitude, double& longitude, double& height ) {
      if( isLatLonOffsetSet ) {
        if( useTM ) {
          TransverseMercator::UTM().Reverse( lon0TM, -y, x + falseNorthingTM, latitude, longitude );
          height = z + height0TM;
        } else {
          _lc.Reverse( -y, x, z, latitude, longitude, height );
        }
      } else {
        latitude = 0;
        longitude = 0;
        height = 0;
//        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
      }
    }

    void Reverse( const double x, const double y, double& latitude, double& longitude, double& height ) {
      if( isLatLonOffsetSet ) {
        if( useTM ) {
          TransverseMercator::UTM().Reverse( lon0TM, -y, x + falseNorthingTM, latitude, longitude );
          height = height0TM;
        } else {
          _lc.Reverse( -y, x, _lc.HeightOrigin(), latitude, longitude, height );
        }
      } else {
        latitude = 0;
        longitude = 0;
        height = 0;
//        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
      }
    }

    void Reset( const double latitude, const double longitude, const double height ) {
      double y;
      lon0TM = longitude;
      TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, y, falseNorthingTM );
//      qDebug() << "TransverseMercatorWrapper::Reset" << this << latitude << longitude << height << useTM << falseNorthingTM << height0TM;

      height0TM = height;

      _lc.Reset( latitude, longitude, height );

      isLatLonOffsetSet = true;
    }

  public:
    bool useTM = true;

  private:
    LocalCartesian _lc;

    bool isLatLonOffsetSet = false;

    double falseNorthingTM = 0;
    double lon0TM = 0;
    double height0TM = 0;
};
