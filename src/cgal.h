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

// all the other CGAL-stuff here

#ifndef CGAL_H
#define CGAL_H

#include "cgalKernel.h"

#ifndef __clang_analyzer__

#include <QDebug>

// standard includes
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdint.h>

typedef int32_t PassNumber;

// functor that defines how to convert PassNumber info when:
// 1. constructing the storage site of an endpoint of a segment
// 2. a segment site is split into two sub-segments
struct PassNumber_convert_info {
  typedef PassNumber    Info;
  typedef const Info&   result_type;
  inline
  const Info& operator()( const Info& info0, bool ) const {
    // just return the info of the supporting segment
    return info0;
  }
  inline
  const Info& operator()( const Info& info0, const Info&, bool ) const {
    // just return the info of the supporting segment
    return info0;
  }
};
// functor that defines how to merge color info when a site (either
// point or segment) corresponds to point(s) on plane belonging to
// more than one input site
struct PassNumber_merge_info {
  typedef PassNumber Info;
  typedef Info       result_type;
  inline
  Info operator()( const Info& info0, const Info& info1 ) const {
    // if the two sites defining the new site have the same info, keep
    // this common info
    if( info0 == info1 ) {
      return info0;
    }

    // otherwise the new site should be purple
    qDebug() << "found not equal Info in the Delauney-graph!" << info0 << info1;
    return info0;
  }
};

// typedefs for the geometric traits, storage traits and the algorithm
#include <CGAL/Segment_Delaunay_graph_2.h>
#include <CGAL/Segment_Delaunay_graph_filtered_traits_2.h>
#include <CGAL/Segment_Delaunay_graph_storage_traits_with_info_2.h>
typedef CGAL::Segment_Delaunay_graph_filtered_traits_2<K> Gt;

// define the storage traits with info
typedef CGAL::Segment_Delaunay_graph_storage_traits_with_info_2<Gt, PassNumber, PassNumber_convert_info, PassNumber_merge_info> ST;
typedef CGAL::Segment_Delaunay_graph_2<Gt, ST>  SDG2;
typedef SDG2::Finite_vertices_iterator  FVIT;
typedef SDG2::Site_2                    Site_2;

#include <CGAL/Segment_Delaunay_graph_adaptation_traits_2.h>
#include <CGAL/Segment_Delaunay_graph_adaptation_policies_2.h>
#include <CGAL/Voronoi_diagram_2.h>
// typedefs for defining the adaptor
typedef CGAL::Segment_Delaunay_graph_adaptation_traits_2<SDG2>                  AT;
typedef CGAL::Segment_Delaunay_graph_caching_degeneracy_removal_policy_2<SDG2>  AP;
typedef CGAL::Voronoi_diagram_2<SDG2, AT, AP>                                     VD;

// typedef for the result type of the point location
typedef AT::Site_2                    AtSite_2;
typedef AT::Point_2                   AtPoint_2;
typedef VD::Locate_result             VdLocate_result;
typedef VD::Vertex_handle             VdVertex_handle;
typedef VD::Face_handle               VdFace_handle;
typedef VD::Halfedge_handle           VdHalfedge_handle;
typedef VD::Ccb_halfedge_circulator   VdCcb_halfedge_circulator;

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Alpha_shape_2.h>
#include <CGAL/Alpha_shape_vertex_base_2.h>
#include <CGAL/Alpha_shape_face_base_2.h>
typedef CGAL::Tag_true                                       Alpha_cmp_tag;
typedef CGAL::Alpha_shape_vertex_base_2<K, CGAL::Default, Alpha_cmp_tag>     AVb;
typedef CGAL::Alpha_shape_face_base_2<K, CGAL::Default, Alpha_cmp_tag>       AFb;
typedef CGAL::Triangulation_data_structure_2<AVb, AFb>        ATds;
typedef CGAL::Delaunay_triangulation_2<K, ATds>               ATriangulation_2;
typedef CGAL::Alpha_shape_2<ATriangulation_2, Alpha_cmp_tag>   Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator            Alpha_shape_edges_iterator;

#include <CGAL/Polyline_simplification_2/simplify.h>
namespace PS = CGAL::Polyline_simplification_2;

#include <CGAL/point_generators_2.h>

#include <CGAL/Bbox_2.h>
typedef CGAL::Bbox_2 Bbox_2;

#include <CGAL/intersections.h>
typedef K::Intersect_2 Intersect_2;

#endif // not __clang_analyzer__

#endif // CGAL_H
