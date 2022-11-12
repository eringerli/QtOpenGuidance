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

#include "GeographicConvertionWrapper.h"

#include <GeographicLib/Ellipsoid.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/TransverseMercator.hpp>
#include <GeographicLib/UTMUPS.hpp>

#include <QtMath>

void
GeographicConvertionWrapper::Forward(
  const double latitude, const double longitude, const double height, double& x, double& y, double& z ) {
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
    _lc.Forward( latitude, longitude, height, y, x, z );
    y = -y;
  }
}

void
GeographicConvertionWrapper::Forward( const double latitude, const double longitude, double& x, double& y, double& z ) {
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

Eigen::Vector3d
GeographicConvertionWrapper::Forward( const Eigen::Vector3d& point ) {
  auto x = double();
  auto y = double();
  auto z = double();

  Forward( point.x(), point.y(), point.z(), x, y, z );

  return Eigen::Vector3d( x, y, z );
}

void
GeographicConvertionWrapper::Reverse(
  const double x, const double y, const double z, double& latitude, double& longitude, double& height ) {
  if( isLatLonOffsetSet ) {
    if( useTM ) {
      TransverseMercator::UTM().Reverse( lon0TM, -y, x + falseNorthingTM, latitude, longitude );
      height = z + height0TM;
    } else {
      _lc.Reverse( -y, x, z, latitude, longitude, height );
    }
  } else {
    latitude  = 0;
    longitude = 0;
    height    = 0;
    //        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
  }
}

void
GeographicConvertionWrapper::Reverse( const double x, const double y, double& latitude, double& longitude, double& height ) {
  if( isLatLonOffsetSet ) {
    if( useTM ) {
      TransverseMercator::UTM().Reverse( lon0TM, -y, x + falseNorthingTM, latitude, longitude );
      height = height0TM;
    } else {
      _lc.Reverse( -y, x, _lc.HeightOrigin(), latitude, longitude, height );
    }
  } else {
    latitude  = 0;
    longitude = 0;
    height    = 0;
    //        qDebug() << "TransverseMercatorWrapper::Reverse: No RESET!!!!";
  }
}

void
GeographicConvertionWrapper::Reset( const double latitude, const double longitude, const double height ) {
  double y;
  lon0TM = longitude;
  TransverseMercator::UTM().Forward( lon0TM, latitude, longitude, y, falseNorthingTM );
  //      qDebug() << "TransverseMercatorWrapper::Reset" << this << latitude << longitude
  //      << height << useTM << falseNorthingTM << height0TM;

  height0TM = height;

  _lc.Reset( latitude, longitude, height );

  isLatLonOffsetSet = true;
}

Eigen::Vector3d
GeographicConvertionWrapper::Reverse( const Eigen::Vector3d& point ) {
  auto x = double();
  auto y = double();
  auto z = double();

  Reverse( point.x(), point.y(), point.z(), x, y, z );

  return Eigen::Vector3d( x, y, z );
}
