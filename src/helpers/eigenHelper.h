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

#include <cmath>

#include "Eigen/Eigen"

#include "unsupported/Eigen/EulerAngles"

using MyTaitBryanSystem = Eigen::EulerSystem < Eigen::EULER_Z, -Eigen::EULER_Y, Eigen::EULER_X >;
using MyTaitBryanAngles = Eigen::EulerAngles<double, MyTaitBryanSystem>;

inline const double& getYaw( const Eigen::Vector3d& vector ) {
  return vector.x();
}

inline double& getYaw( Eigen::Vector3d& vector ) {
  return vector.x();
}

inline const double& getPitch( const Eigen::Vector3d& vector ) {
  return vector.y();
}

inline double& getPitch( Eigen::Vector3d& vector ) {
  return vector.y();
}

inline const double& getRoll( const Eigen::Vector3d& vector ) {
  return vector.z();
}

inline double& getRoll( Eigen::Vector3d& vector ) {
  return vector.z();
}

inline Eigen::Quaterniond taitBryanToQuaternion( const Eigen::Vector3d& eulers ) {
  MyTaitBryanAngles euler( eulers );
  return euler;
}

inline Eigen::Quaterniond taitBryanToQuaternion( const double yaw, const double pitch, const double roll ) {
  Eigen::Vector3d vec( yaw, pitch, roll );
  return taitBryanToQuaternion( vec );
}

inline Eigen::Vector3d quaternionToTaitBryan( const Eigen::Quaterniond& quaternion ) {
  MyTaitBryanAngles euler( quaternion );
  return euler.angles();
}

#ifdef QT_VERSION

#include <QQuaternion>

inline Eigen::Quaterniond toEigenQuaternion( const QQuaternion& quaternion ) {
  return Eigen::Quaterniond( quaternion.scalar(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline QQuaternion toQQuaternion( const Eigen::Quaterniond& quaternion ) {
  return QQuaternion( quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline Eigen::Vector3d toEigenVector( QVector3D& vec ) {
  return Eigen::Vector3d( vec.x(), vec.y(), vec.z() );
}

inline QVector3D toQVector3D( const Eigen::Vector3d& vec ) {
  return QVector3D( float( vec.x() ), float( vec.y() ), float( vec.z() ) );
}

#endif
