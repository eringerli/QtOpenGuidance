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

#include <QGeometryRenderer>
#include <QVector3D>
#include <QVector4D>
#include "CultivatedAreaMesh.h"
#include "CultivatedAreaMeshGeometry.h"

CultivatedAreaMesh::CultivatedAreaMesh( Qt3DCore::QNode* parent ) :
  Qt3DRender::QGeometryRenderer( parent ),
  m_trackMeshGeometry( new CultivatedAreaMeshGeometry( this ) ) {
  setInstanceCount( 1 );
  setIndexOffset( 0 );
  setFirstInstance( 0 );
  setPrimitiveType( Qt3DRender::QGeometryRenderer::Triangles );
  setGeometry( m_trackMeshGeometry );

  QObject::connect( m_trackMeshGeometry, &CultivatedAreaMeshGeometry::vertexCountChanged, this, &QGeometryRenderer::setVertexCount );
}

CultivatedAreaMesh::~CultivatedAreaMesh() {
  m_trackMeshGeometry->deleteLater();
}

void CultivatedAreaMesh::addPoints( const Point_2 point1, const Point_2 point2 ) {
  m_trackMeshGeometry->addPoints( point1, point2 );
}

void CultivatedAreaMesh::addPointLeft( const Point_2 point ) {
  m_trackMeshGeometry->addPointLeft( point );
}

void CultivatedAreaMesh::addPointRight( const Point_2 point ) {
  m_trackMeshGeometry->addPointRight( point );
}

void CultivatedAreaMesh::addTrackMesh( CultivatedAreaMesh* trackMesh ) {
  m_trackMeshGeometry->addTrackMeshGeometry( trackMesh->m_trackMeshGeometry );
}

void CultivatedAreaMesh::optimise( CgalThread* thread ) {
  m_trackMeshGeometry->optimise( thread );
}
