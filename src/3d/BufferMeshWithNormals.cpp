// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMeshWithNormals.h"
#include "BufferMeshGeometryViewWithNormals.h"

#include <QVector3D>
#include <QVector>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>

BufferMeshWithNormals::BufferMeshWithNormals( Qt3DCore::QNode* parent ) : Qt3DRender::QGeometryRenderer( parent ) {
  auto* geometryView = new BufferMeshGeometryViewWithNormals( this );
  QGeometryRenderer::setView( geometryView );
}

BufferMeshWithNormals::~BufferMeshWithNormals() {}

void
BufferMeshWithNormals::bufferUpdate( const QVector< QVector3D >& pos ) {
  static_cast< BufferMeshGeometryViewWithNormals* >( view() )->bufferUpdate( pos );
}
