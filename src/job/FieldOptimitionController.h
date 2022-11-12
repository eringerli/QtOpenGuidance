// Copyright( C ) 2021 Christian Riggenbach
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

#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/ThreadWeaver>

#include <QObject>
#include <QPointer>

#include "gui/FieldsOptimitionToolbar.h"
#include "helpers/cgalHelper.h"

#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
typedef CGAL::Tag_true                                                         Alpha_cmp_tag;
typedef CGAL::Alpha_shape_vertex_base_2< Epick, CGAL::Default, Alpha_cmp_tag > AVb;
typedef CGAL::Alpha_shape_face_base_2< Epick, CGAL::Default, Alpha_cmp_tag >   AFb;
typedef CGAL::Triangulation_data_structure_2< AVb, AFb >                       ATds;
typedef CGAL::Delaunay_triangulation_2< Epick, ATds >                          ATriangulation_2;
typedef CGAL::Alpha_shape_2< ATriangulation_2, Alpha_cmp_tag >                 Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator                              Alpha_shape_edges_iterator;

class FieldOptimitionController : public QObject {
  Q_OBJECT
public:
  FieldOptimitionController() = delete;

  FieldOptimitionController( std::shared_ptr< std::vector< Point_2 > > pointsPointer );
  virtual ~FieldOptimitionController();

  void run( const FieldsOptimitionToolbar::AlphaType alphaType,
            const double                             customAlpha,
            const double                             maxDeviation,
            const double                             distanceBetweenConnectPoints );

  void stop();

private:
  ThreadWeaver::JobPointer sequence;

  std::shared_ptr< Alpha_shape_2 >        alphaShape = nullptr;
  std::shared_ptr< Polygon_with_holes_2 > out_poly;

  std::shared_ptr< std::vector< Point_2 > > pointsPointer;

  double optimalAlpha = 0;
  double solidAlpha   = 0;

  static int counter;

private:
  void alphaToPolygon();

Q_SIGNALS:
  void alphaChanged( const double optimal, const double solid );
  void fieldStatisticsChanged( const double, const double, const double );
  void fieldOptimitionFinished( std::shared_ptr< Polygon_with_holes_2 >, const double );
};
