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

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include "../helpers/cgalHelper.h"
#include "../gui/FieldsOptimitionToolbar.h"

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
typedef CGAL::Tag_true                                                            Alpha_cmp_tag;
typedef CGAL::Alpha_shape_vertex_base_2<Epick, CGAL::Default, Alpha_cmp_tag>      AVb;
typedef CGAL::Alpha_shape_face_base_2<Epick, CGAL::Default, Alpha_cmp_tag>        AFb;
typedef CGAL::Triangulation_data_structure_2<AVb, AFb>                            ATds;
typedef CGAL::Delaunay_triangulation_2<Epick, ATds>                               ATriangulation_2;
typedef CGAL::Alpha_shape_2<ATriangulation_2, Alpha_cmp_tag>                      Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator                                 Alpha_shape_edges_iterator;

class CgalWorker : public QObject {
    Q_OBJECT
  public:
    explicit CgalWorker( QObject* parent = nullptr );


  public Q_SLOTS:
    void fieldOptimitionWorker( const uint32_t runNumber,
                                std::vector<Point_2>* points,
                                const FieldsOptimitionToolbar::AlphaType alphaType,
                                const double customAlpha,
                                const double maxDeviation,
                                const double distanceBetweenConnectPoints );

    bool isCollinear( std::vector<Point_2>* pointsPointer, const bool emitSignal = false );
    void connectPoints( std::vector<Point_2>* pointsPointer, const double distanceBetweenConnectPoints, const bool emitSignal = false );
    void simplifyPolygon( Polygon_with_holes_2* out_poly, const double maxDeviation, const bool emitSignal = false );
    void simplifyPolyline( std::vector<Point_2>* pointsPointer, const double maxDeviation );

  Q_SIGNALS:
    void alphaShapeFinished( std::shared_ptr<Polygon_with_holes_2>, const double );
    void alphaChanged( const double optimal, const double solid );
    void fieldStatisticsChanged( const double, const double, const double );

    void isCollinearResult( const bool );
    void connectPointsResult( std::vector<Point_2>* );
    void simplifyPolygonResult( Polygon_with_holes_2* );
    void simplifyPolylineResult( std::vector<Point_2>* );

  private:
    // form polygons from alpha shape
    void alphaToPolygon( const Alpha_shape_2& A,
                         Polygon_with_holes_2& out_poly );

    bool returnEarly( const uint32_t runNumber );
};

class CgalThread : public QThread {
    Q_OBJECT
  public:
    explicit CgalThread( QObject* parent = nullptr )
      : QThread( parent ) {}

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
    QMutex mutex;
    uint32_t runNumber = 0;
};

Q_DECLARE_METATYPE( FieldsOptimitionToolbar::AlphaType )
Q_DECLARE_METATYPE( uint32_t )
Q_DECLARE_METATYPE( std::shared_ptr<Polygon_with_holes_2> )
