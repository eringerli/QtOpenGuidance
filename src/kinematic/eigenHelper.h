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

// only "lightweight" CGAL-stuff here!

#pragma once

#include <cmath>

#include "Eigen/Eigen"
#include "Eigen/Geometry"

#include <QQuaternion>

inline Eigen::Quaterniond eulerToQuaternion( double roll, double pitch, double yaw ) {
  return Eigen::AngleAxisd( roll, Eigen::Vector3d::UnitX() )
         * Eigen::AngleAxisd( pitch, Eigen::Vector3d::UnitY() )
         * Eigen::AngleAxisd( yaw, Eigen::Vector3d::UnitZ() );
}

inline Eigen::Vector3d quaternionToEuler( Eigen::Quaterniond quaternion ) {
  return quaternion.toRotationMatrix().eulerAngles( 0, 1, 2 );
}

inline Eigen::Quaterniond toEigenQuaternion( QQuaternion quaternion ) {
  return Eigen::Quaterniond( quaternion.scalar(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline QQuaternion toQQuaternion( Eigen::Quaterniond quaternion ) {
  return QQuaternion( quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline Eigen::Vector3d toEigenVector3( QVector3D vec ) {
  return Eigen::Vector3d( vec.x(), vec.y(), vec.z() );
}

//#endif // not __clang_analyzer__
