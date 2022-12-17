// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMetaType>

#include "kinematic/cgalKernel.h"

#include "helpers/anglesHelper.h"

// basic conversions
inline const QVector3D
toQVector3D( const Point_3& point ) {
  return QVector3D( float( point.x() ), float( point.y() ), float( point.z() ) );
}

inline const QVector3D
toQVector3D( const Vector_3& vector ) {
  return QVector3D( float( vector.x() ), float( vector.y() ), float( vector.z() ) );
}

inline const QVector3D
toQVector3D( Point_2 point, const float z = 0 ) {
  return QVector3D( float( point.x() ), float( point.y() ), z );
}

inline const Point_3
toPoint3( QVector3D point ) {
  return Point_3( point.x(), point.y(), point.z() );
}

inline const Point_3
toPoint3( const Eigen::Vector3d& point ) {
  return Point_3( point.x(), point.y(), point.z() );
}

inline const Point_2
toPoint2( const Eigen::Vector3d& point ) {
  return Point_2( point.x(), point.y() );
}

inline const Segment_2
to2D( const Segment_3& segment ) {
  return Segment_2( Point_2( segment.source().x(), segment.source().y() ), Point_2( segment.target().x(), segment.target().y() ) );
}

inline const Point_2
to2D( const Point_3& point ) {
  return Point_2( point.x(), point.y() );
}

inline const Point_2
to2D( const Eigen::Vector3d& point ) {
  return Point_2( point.x(), point.y() );
}

inline const Point_3
removeZ( const Point_3& point ) {
  return Point_3( point.x(), point.y(), 0 );
}

inline const Eigen::Vector3d
removeZ( const Eigen::Vector3d& point ) {
  return Eigen::Vector3d( point.x(), point.y(), 0 );
}

inline const Point_3
to3D( Point_2 point ) {
  return Point_3( point.x(), point.y(), 0 );
}

inline const Segment_3
to3D( const Segment_2& segment ) {
  return Segment_3( Point_3( segment.source().x(), segment.source().y(), 0 ), Point_3( segment.target().x(), segment.target().y(), 0 ) );
}

inline const Vector_3
to3D( Vector_2 vector ) {
  return Vector_3( vector.x(), vector.y(), 0 );
}

inline const Eigen::Vector3d
toEigenVector( const Vector_3& vector ) {
  return Eigen::Vector3d( vector.x(), vector.y(), vector.z() );
}

inline const Eigen::Vector3d
toEigenVector( const Point_3& point ) {
  return Eigen::Vector3d( point.x(), point.y(), point.z() );
}

inline const Eigen::Vector3d
toEigenVector( Point_2 point ) {
  return Eigen::Vector3d( point.x(), point.y(), 0 );
}

inline const Vector_3
toCgalVector( const Eigen::Vector3d& vector ) {
  return Vector_3( vector.x(), vector.y(), vector.z() );
}

inline Vector_2
polarOffsetRad( const double angleRad, const double distance ) {
  return Vector_2( std::sin( angleRad ) * distance, -std::cos( angleRad ) * distance );
}

inline Vector_2
polarOffsetDegrees( const double angleDegrees, const double distance ) {
  double angleRad = degreesToRadians( angleDegrees );
  return Vector_2( std::sin( angleRad ) * distance, -std::cos( angleRad ) * distance );
}

inline double
angleOfLineDegrees( const Line_2& line ) {
  Vector_3 north( 1, 0, 0 );
  Vector_2 north2D( 1, 0 );

  Vector_2 lineVector = line.to_vector();

  double angleAB = CGAL::approximate_angle( north, ( Vector_3 )to3D( lineVector ) );
  return normalizeAngleDegrees( ( angleAB ) * ( CGAL::orientation( north2D, lineVector ) == CGAL::Sign::POSITIVE ? 1 : -1 ) );
}

inline double
angleOfLineRadians( const Line_2& line ) {
  return degreesToRadians( angleOfLineDegrees( line ) );
}

void registerCgalTypes();

Q_DECLARE_METATYPE( uint32_t )

Q_DECLARE_METATYPE( Point_2 )
Q_DECLARE_METATYPE( Point_3 )
Q_DECLARE_METATYPE( std::shared_ptr< std::vector< Point_2 > > )
Q_DECLARE_METATYPE( std::shared_ptr< std::vector< Point_3 > > )
Q_DECLARE_METATYPE( std::shared_ptr< std::vector< QVector3D > > )
Q_DECLARE_METATYPE( std::shared_ptr< Polygon_with_holes_2 > )

// Q_DECLARE_METATYPE( std::shared_ptr< SurfaceMesh_3 > )
