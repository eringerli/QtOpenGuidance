// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMesh.h"
#include "BufferMeshGeometryView.h"
#include <QVector3D>
#include <QVector4D>

BufferMesh::BufferMesh( Qt3DCore::QNode* parent ) : Qt3DRender::QGeometryRenderer( parent ) {
  auto* geometryView = new BufferMeshGeometryView( this );
  QGeometryRenderer::setView( geometryView );
}

BufferMesh::~BufferMesh() {}

void
BufferMesh::bufferUpdate( const QVector< QVector3D >& pos ) {
  static_cast< BufferMeshGeometryView* >( view() )->bufferUpdate( pos );
}
