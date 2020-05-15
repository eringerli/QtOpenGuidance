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

#include "../cgalKernel.h"
#include "../gui/FieldsOptimitionToolbar.h"

#include <QSharedPointer>

class CgalWorker : public QObject {
    Q_OBJECT
  public:
    explicit CgalWorker( QObject* parent = nullptr );


  public slots:
    void fieldOptimitionWorker( uint32_t runNumber,
                                std::vector<Point_2>* points,
                                FieldsOptimitionToolbar::AlphaType alphaType,
                                double customAlpha,
                                double maxDeviation,
                                double distanceBetweenConnectPoints );

    bool isCollinear( std::vector<Point_2>* pointsPointer, bool emitSignal = false );
    void connectPoints( std::vector<Point_2>* pointsPointer, double distanceBetweenConnectPoints, bool emitSignal = false );
    void simplifyPolygon( Polygon_with_holes_2* out_poly, double maxDeviation, bool emitSignal = false );
    void simplifyPolyline( std::vector<Point_2>* pointsPointer, double maxDeviation, bool emitSignal = false );

  signals:
    void alphaShapeFinished( std::shared_ptr<Polygon_with_holes_2>, double );
    void alphaChanged( double optimal, double solid );
    void fieldStatisticsChanged( double, double, double );

    void isCollinearResult( bool );
    void connectPointsResult( std::vector<Point_2>* );
    void simplifyPolygonResult( Polygon_with_holes_2* );
    void simplifyPolylineResult( std::vector<Point_2>* );

  private:
    // form polygons from alpha shape
    void alphaToPolygon( const Alpha_shape_2& A,
                         Polygon_with_holes_2& out_poly );

    bool returnEarly( uint32_t runNumber );
};

class CgalThread : public QThread {
    Q_OBJECT
  public:
    explicit CgalThread( QObject* parent = nullptr )
      : QThread( parent ) {}

  public slots:
    void requestNewRunNumber() {
      {
        QMutexLocker lock( &mutex );
        ++runNumber;
      }
      emit runNumberChanged( runNumber );
    }

  signals:
    void runNumberChanged( uint32_t );

  public:
    QMutex mutex;
    uint32_t runNumber = 0;
};

Q_DECLARE_METATYPE( FieldsOptimitionToolbar::AlphaType )
Q_DECLARE_METATYPE( uint32_t )
Q_DECLARE_METATYPE( std::shared_ptr<Polygon_with_holes_2> )
