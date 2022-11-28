// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cmath>

#include "Eigen/Eigen"

#include "unsupported/Eigen/EulerAngles"

using MyTaitBryanSystem = Eigen::EulerSystem< Eigen::EULER_Z, -Eigen::EULER_Y, Eigen::EULER_X >;
using MyTaitBryanAngles = Eigen::EulerAngles< double, MyTaitBryanSystem >;

inline const double&
getYaw( const Eigen::Vector3d& vector ) {
  return vector.x();
}

inline double&
getYaw( Eigen::Vector3d& vector ) {
  return vector.x();
}

inline const double&
getPitch( const Eigen::Vector3d& vector ) {
  return vector.y();
}

inline double&
getPitch( Eigen::Vector3d& vector ) {
  return vector.y();
}

inline const double&
getRoll( const Eigen::Vector3d& vector ) {
  return vector.z();
}

inline double&
getRoll( Eigen::Vector3d& vector ) {
  return vector.z();
}

inline Eigen::Quaterniond
taitBryanToQuaternion( const Eigen::Vector3d& eulers ) {
  MyTaitBryanAngles euler( getYaw( eulers ), getPitch( eulers ), getRoll( eulers ) );
  return euler;
}

inline Eigen::Quaterniond
taitBryanToQuaternion( const double yaw, const double pitch, const double roll ) {
  MyTaitBryanAngles euler( yaw, pitch, roll );
  return euler;
}

inline Eigen::Vector3d
quaternionToTaitBryan( const Eigen::Quaterniond& quaternion ) {
  return MyTaitBryanAngles( quaternion ).angles();
}

inline double
quaternionToTaitBryanYaw( const Eigen::Quaterniond& quaternion ) {
  return MyTaitBryanAngles( quaternion ).alpha();
}

inline Eigen::Vector3d
getNormalFromQuaternion( Eigen::Quaterniond orientation ) {
  return orientation._transformVector( Eigen::Vector3d::UnitZ() );
}

inline Eigen::Quaterniond
getAbsoluteOrientation( Eigen::Quaterniond orientation ) {
  return Eigen::Quaterniond::FromTwoVectors( Eigen::Vector3d::UnitZ(), getNormalFromQuaternion( orientation ) );
}

namespace EigenHelper {
  extern Eigen::IOFormat CommaInitFmt;
}

inline auto
printVector( const Eigen::Vector2d& vector ) {
  return vector.format( EigenHelper::CommaInitFmt );
}

inline auto
printVector( const Eigen::Vector3d& vector ) {
  return vector.format( EigenHelper::CommaInitFmt );
}

inline auto
printVector( const Eigen::Vector4d& vector ) {
  return vector.format( EigenHelper::CommaInitFmt );
}

inline auto
printMatrix( const Eigen::Matrix2d& matrix ) {
  return matrix.format( EigenHelper::CommaInitFmt );
}

inline auto
printQuaternion( const Eigen::Quaterniond& quaternion ) {
  Eigen::Vector4d vec( quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z() );
  return printVector( vec );
}

inline auto
printQuaternionAsTaitBryanRadians( const Eigen::Quaterniond& quaternion ) {
  return quaternionToTaitBryan( quaternion ).format( EigenHelper::CommaInitFmt );
}

inline auto
printQuaternionAsTaitBryanDegrees( const Eigen::Quaterniond& quaternion ) {
  return ( quaternionToTaitBryan( quaternion ) * 180 / M_PI ).format( EigenHelper::CommaInitFmt );
}

#ifdef QT_VERSION
  #include <QMetaType>
  #include <QQuaternion>

inline Eigen::Quaterniond
toEigenQuaternion( const QQuaternion& quaternion ) {
  return Eigen::Quaterniond( quaternion.scalar(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline QQuaternion
toQQuaternion( const Eigen::Quaterniond& quaternion ) {
  return QQuaternion( quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z() );
}

inline Eigen::Vector3d
toEigenVector( QVector3D& vec ) {
  return Eigen::Vector3d( vec.x(), vec.y(), vec.z() );
}

inline QVector3D
toQVector3D( const Eigen::Vector3d& vec ) {
  return QVector3D( float( vec.x() ), float( vec.y() ), float( vec.z() ) );
}

Q_DECLARE_METATYPE( float );
Q_DECLARE_METATYPE( double );

Q_DECLARE_METATYPE( Eigen::Vector2d )
Q_DECLARE_METATYPE( Eigen::Vector3d )
Q_DECLARE_METATYPE( Eigen::Vector4d )

Q_DECLARE_METATYPE( Eigen::Quaterniond )

#endif
