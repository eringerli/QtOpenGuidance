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

#include <GeographicLib/TransverseMercator.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <GeographicLib/Ellipsoid.hpp>

// define the symbol USE_TRANSVERSEMERCATOR tu use a transverse mercator-projection centered on the first
// longitude, set byGeographicConvertionWrapper::Reset(). If not defined, it uses the normal UTM-projection.
// This introduces deviations in heading, the farther you get from the center, so it is not recomended.
#define USE_TRANSVERSEMERCATOR

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

//        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward Reset" << latitude << longitude << height << useTM;
        x = 0;
        y = 0;
        z = 0;

        return;
      }

      if( useTM ) {
#ifdef USE_TRANSVERSEMERCATOR
        TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, x, y );
#else
        int zone;
        bool northp;
        UTMUPS::Forward( latitude, longitude, zone, northp, x, y );
        x -= falseEastingUTM;
#endif
        y -= falseNorthingTM;
        y = -y;
        z = height - height0TM;
      } else {
        _lc.Forward( latitude, longitude,  height, x, y, z );
        y = -y;
      }

//      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward" << latitude << longitude << height << useTM << x << y << z << _lc.LatitudeOrigin() << _lc.LongitudeOrigin() << _lc.HeightOrigin();
    }

    void Forward( const double latitude, const double longitude, double& x, double& y, double& z ) {
      if( !isLatLonOffsetSet ) {
        Reset( latitude, longitude, height0TM );

//        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward Reset" << latitude << longitude << useTM;
        x = 0;
        y = 0;
        z = 0;

        return;
      }

      if( useTM ) {
#ifdef USE_TRANSVERSEMERCATOR
        TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, x, y );
#else
        int zone;
        bool northp;
        UTMUPS::Forward( latitude, longitude, zone, northp, x, y );
        x -= falseEastingUTM;
#endif
        y -= falseNorthingTM;
        y = -y;
        z = 0;
      } else {
        _lc.Forward( latitude, longitude, _lc.HeightOrigin(), x, y, z );
        y = -y;
      }

//      qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Forward" << latitude << longitude << useTM << x << y << z << _lc.LatitudeOrigin() << _lc.LongitudeOrigin() << _lc.HeightOrigin();
    }

    void Reverse( const double x, const double y, const double z, double& latitude, double& longitude, double& height ) {
      if( isLatLonOffsetSet ) {
        if( useTM ) {
#ifdef USE_TRANSVERSEMERCATOR
          TransverseMercator::UTM().Reverse( lon0TM, x, -y + falseNorthingTM, latitude, longitude );
#else
          UTMUPS::Reverse( zoneUTM, northUTM, x + falseEastingUTM, -y + falseNorthingTM, latitude, longitude );
#endif
          height = z + height0TM;
        } else {
          _lc.Reverse( x, -y, z, latitude, longitude, height );
        }

//        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Reverse" << latitude << longitude << height << useTM << x << y << z;
      } else {
        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
      }
    }

    void Reverse( const double x, const double y, double& latitude, double& longitude, double& height ) {
      if( isLatLonOffsetSet ) {
        if( useTM ) {
#ifdef USE_TRANSVERSEMERCATOR
          TransverseMercator::UTM().Reverse( lon0TM, x, -y + falseNorthingTM, latitude, longitude );
#else
          UTMUPS::Reverse( zoneUTM, northUTM, x + falseEastingUTM, -y + falseNorthingTM, latitude, longitude );
#endif
          height = height0TM;
        } else {
          _lc.Reverse( x, -y, _lc.HeightOrigin(), latitude, longitude, height );
        }

//        qDebug() << qSetRealNumberPrecision( 10 ) << "TransverseMercatorWrapper::Reverse" << latitude << longitude << height << useTM << x << y;
      } else {
        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
      }
    }

    void Reset( const double latitude, const double longitude, const double height ) {
      // set the false Easting/Norting and zone
      //      falseEastingUTM = x;
      //      falseNorthingTM = y;
#ifdef USE_TRANSVERSEMERCATOR
      double x;
      lon0TM = longitude;
      TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, x, falseNorthingTM );
      qDebug() << "TransverseMercatorWrapper::Reset" << this << latitude << longitude << height << useTM << falseNorthingTM << height0TM;
#else
      UTMUPS::Forward( latitude, longitude, zoneUTM, northUTM, falseEastingUTM, falseNorthingTM );
      qDebug() << "TransverseMercatorWrapper::Reset" << this << latitude << longitude << height << useTM << falseNorthingTM << falseEastingUTM << height0TM;
#endif
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
#ifdef USE_TRANSVERSEMERCATOR
    double lon0TM = 0;
#else
    double falseEastingUTM = 0;
    int zoneUTM = -2;
    bool northUTM = true;
#endif
    double height0TM = 0;
};

#endif // GEOGRAPHICCONVERTIONWRAPPER_H
