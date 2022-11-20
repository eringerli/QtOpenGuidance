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

#include "CultivatedAreaMesh.h"
#include "CultivatedAreaMeshGeometryView.h"
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>

CultivatedAreaMesh::CultivatedAreaMesh( Qt3DCore::QNode* parent ) : Qt3DRender::QGeometryRenderer( parent ) {
  auto* geometryView = new CultivatedAreaMeshGeometryView( this );
  QGeometryRenderer::setView( geometryView );
}

CultivatedAreaMesh::~CultivatedAreaMesh() { view()->setEnabled( false ); }

void
CultivatedAreaMesh::addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPoints( point1, point2, normalVector );
}

void
CultivatedAreaMesh::addPointLeft( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPointLeft( point, normalVector );
}

void
CultivatedAreaMesh::addPointRight( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPointRight( point, normalVector );
}

void
CultivatedAreaMesh::clear() {
  qDebug() << "CultivatedAreaMesh::clear()";
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->clear();
}

void
CultivatedAreaMesh::addTrackMesh( CultivatedAreaMesh* trackMesh ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addTrackMesh(
    static_cast< CultivatedAreaMeshGeometryView* >( trackMesh->view() ) );
}

void
CultivatedAreaMesh::optimise() {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->optimise();
}

size_t
CultivatedAreaMesh::numPoints() const {
  return static_cast< CultivatedAreaMeshGeometryView* >( view() )->numPoints();
}
