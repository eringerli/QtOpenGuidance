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

#ifndef CGALWORKER_H
#define CGALWORKER_H

#include <QtCore/QObject>

#include "../cgalKernel.h"
#include "../gui/FieldsOptimitionToolbar.h"

#include <QSharedPointer>

class CgalWorker : public QObject {
    Q_OBJECT
  public:
    explicit CgalWorker( QObject* parent = nullptr );

  private:
    // form polygons from alpha shape
    void alphaToPolygon( const Alpha_shape_2& A,
                         Polygon_with_holes_2& out_poly );

  public slots:
    void fieldOptimitionWorker( std::vector<Point_2>* points,
                                FieldsOptimitionToolbar::AlphaType alphaType,
                                double customAlpha,
                                double maxDeviation,
                                double distanceBetweenConnectPoints );

  signals:
    void alphaShapeFinished( Polygon_with_holes_2* );
    void alphaChanged( double optimal, double solid );
    void fieldStatisticsChanged( double, double, double );
};

Q_DECLARE_METATYPE( FieldsOptimitionToolbar::AlphaType )

#endif // CGALWORKER_H
