// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>

#include "gui/FieldsOptimitionToolbar.h"
#include "helpers/cgalHelper.h"

#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
using Alpha_cmp_tag              = CGAL::Tag_true;
using AVb                        = CGAL::Alpha_shape_vertex_base_2< Epick, CGAL::Default, Alpha_cmp_tag >;
using AFb                        = CGAL::Alpha_shape_face_base_2< Epick, CGAL::Default, Alpha_cmp_tag >;
using ATds                       = CGAL::Triangulation_data_structure_2< AVb, AFb >;
using ATriangulation_2           = CGAL::Delaunay_triangulation_2< Epick, ATds >;
using Alpha_shape_2              = CGAL::Alpha_shape_2< ATriangulation_2, Alpha_cmp_tag >;
using Alpha_shape_edges_iterator = Alpha_shape_2::Alpha_shape_edges_iterator;

class CgalWorker : public QObject {
  Q_OBJECT
public:
  explicit CgalWorker( QObject* parent = nullptr );

public Q_SLOTS:
  void fieldOptimitionWorker( const uint32_t                           runNumber,
                              std::vector< Point_2 >*                  points,
                              const FieldsOptimitionToolbar::AlphaType alphaType,
                              const double                             customAlpha,
                              const double                             maxDeviation,
                              const double                             distanceBetweenConnectPoints );

  bool isCollinear( std::vector< Point_2 >* pointsPointer, const bool emitSignal = false );
  void connectPoints( std::vector< Point_2 >* pointsPointer, const double distanceBetweenConnectPoints, const bool emitSignal = false );
  void simplifyPolygon( Polygon_with_holes_2* out_poly, const double maxDeviation, const bool emitSignal = false );
  void simplifyPolyline_2( std::vector< Point_2 >* pointsPointer, const double maxDeviation );
  void simplifyPolyline_3( std::vector< Point_3 >* pointsPointer, const double maxDeviation );

Q_SIGNALS:
  void alphaShapeFinished( std::shared_ptr< Polygon_with_holes_2 >, const double );
  void alphaChanged( const double optimal, const double solid );
  void fieldStatisticsChanged( const double, const double, const double );

  void isCollinearResult( const bool );
  void connectPointsResult( std::vector< Point_2 >* );
  void simplifyPolygonResult( Polygon_with_holes_2* );
  void simplifyPolylineResult_2( std::vector< Point_2 >* );
  void simplifyPolylineResult_3( std::vector< Point_3 >* );

private:
  // form polygons from alpha shape
  void alphaToPolygon( const Alpha_shape_2& A, Polygon_with_holes_2& out_poly );

  bool returnEarly( const uint32_t runNumber );
};

class CgalThread : public QThread {
  Q_OBJECT
public:
  explicit CgalThread( QObject* parent = nullptr ) : QThread( parent ) {}

public Q_SLOTS:
  void requestNewRunNumber() {
    {
      QMutexLocker lock( &mutex );
      ++runNumber;
    }
    Q_EMIT runNumberChanged( runNumber );
  }

Q_SIGNALS:
  void runNumberChanged( uint32_t );

public:
  QMutex   mutex;
  uint32_t runNumber = 0;
};

Q_DECLARE_METATYPE( FieldsOptimitionToolbar::AlphaType )
