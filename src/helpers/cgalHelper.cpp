// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "helpers/cgalHelper.h"

#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Surface_mesh.h>
using SurfaceMesh_3 = CGAL::Surface_mesh< Point_3 >;

void
registerCgalTypes() {
  qRegisterMetaType< uint32_t >();

  qRegisterMetaType< Point_2 >();
  qRegisterMetaType< Point_3 >();

  qRegisterMetaType< std::shared_ptr< std::vector< Point_2 > > >();
  qRegisterMetaType< std::shared_ptr< std::vector< Point_3 > > >();
  qRegisterMetaType< std::shared_ptr< std::vector< QVector3D > > >();
  qRegisterMetaType< std::shared_ptr< Polygon_with_holes_2 > >();

  qRegisterMetaType< std::shared_ptr< SurfaceMesh_3 > >();
}
