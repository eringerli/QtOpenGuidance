// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef TRANSVERSEMERCATORWRAPPER_H
#define TRANSVERSEMERCATORWRAPPER_H

#include <GeographicLib/TransverseMercator.hpp>

using namespace std;
using namespace GeographicLib;

// an instance of this class gets shared across all the blocks, so the conversions are the same everywhere
class TransverseMercatorWrapper {
  public:

    TransverseMercatorWrapper()
      : _tm( Constants::WGS84_a(), Constants::WGS84_f(), Constants::UTM_k0() ) {
    }

    void Forward( double latitude, double longitude, double& height, double& x, double& y ) {
      if( !isLatLonOffsetSet ) {
        lat0 = latitude;
        lon0 = longitude;
        height0 = height;
        isLatLonOffsetSet = true;
      }

      latitude -= lat0;
      height -= height0;

      double convergence;
      double scale;
      _tm.Forward( lon0, latitude, longitude, y, x, convergence, scale );
//      qDebug() << "lat0, lon0, isLatLonOffsetSet" << lat0 << lon0 << isLatLonOffsetSet;
//      qDebug() << x << y << convergence << scale;
    }

    void Reverse( double x, double y, double& latitude, double& longitude, double& height ) {
      double convergence;
      double scale;
      _tm.Reverse( lon0, y, x, latitude, longitude, convergence, scale );

      latitude += lat0;
      height += height0;
      qDebug() << lat0 << lon0;
      qDebug() << x << y << convergence << scale;
    }

  private:
    TransverseMercator _tm;

    bool isLatLonOffsetSet = false;
    double height0 = 0;
    double lat0 = 0;
    double lon0 = 0;
};

#endif // TRANSVERSEMERCATORWRAPPER_H
