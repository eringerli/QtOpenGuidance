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

#ifndef GEOGRAPHICCONVERTIONWRAPPER_H
#define GEOGRAPHICCONVERTIONWRAPPER_H

#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/Ellipsoid.hpp>

using namespace std;
using namespace GeographicLib;

// an instance of this class gets shared across all the blocks, so the conversions are the same everywhere
class GeographicConvertionWrapper {
  public:

    GeographicConvertionWrapper()
      : _tm() {
    }

    void Forward( const double latitude, const double longitude, const double height, double& x, double& y, double& z ) {
      if( !isLatLonOffsetSet ) {
        _tm.Reset( latitude, longitude, height );

        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward Reset" << latitude << longitude << height;
        x = 0;
        y = 0;
        z = 0;

        isLatLonOffsetSet = true;

        return;
      }

      _tm.Forward( latitude, longitude,  height, y, x, z );
      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward" << latitude << longitude << height << x << y << z << _tm.LatitudeOrigin() << _tm.LongitudeOrigin() << _tm.HeightOrigin();
    }

    void Forward( const double latitude, const double longitude, double& x, double& y, double& z ) {
      if( !isLatLonOffsetSet ) {
        _tm.Reset( latitude, longitude, _tm.HeightOrigin() );

        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward Reset" << latitude << longitude;
        x = 0;
        y = 0;
        z = 0;

        isLatLonOffsetSet = true;

        return;
      }

      _tm.Forward( latitude, longitude, _tm.HeightOrigin(), y, x, z );
      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward" << latitude << longitude << x << y << z << _tm.LatitudeOrigin() << _tm.LongitudeOrigin() << _tm.HeightOrigin();
    }

    void Reverse( const double x, const double y, const double z, double& latitude, double& longitude, double& height ) {
      _tm.Reverse( x, y, z, latitude, longitude, height );
      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Reverse" << latitude << longitude << height << x << y << z;
    }

    void Reverse( const double x, const double y, double& latitude, double& longitude, double& height ) {
      _tm.Reverse( x, y, _tm.HeightOrigin(), latitude, longitude, height );
      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Reverse" << latitude << longitude << height << x << y;
    }

    void Reset( const double latitude, const double longitude, const double height ) {
      _tm.Reset( latitude, longitude, height );
    }

  private:
    LocalCartesian _tm;

    bool isLatLonOffsetSet = false;
};

#endif // GEOGRAPHICCONVERTIONWRAPPER_H
