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
