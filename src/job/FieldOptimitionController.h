// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/ThreadWeaver>

#include <QObject>
#include <QPointer>

#include "gui/toolbar/FieldsOptimitionToolbar.h"
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
