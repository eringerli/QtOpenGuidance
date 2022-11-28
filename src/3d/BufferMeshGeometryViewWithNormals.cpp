// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMeshGeometryViewWithNormals.h"
#include "BufferMeshGeometryWithNormals.h"

#include <QVector3D>
#include <QVector>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>

BufferMeshGeometryViewWithNormals::BufferMeshGeometryViewWithNormals( Qt3DCore::QNode* parent ) : Qt3DCore::QGeometryView( parent ) {
  auto* geometry = new BufferMeshGeometryWithNormals( this );
  QGeometryView::setGeometry( geometry );
}

BufferMeshGeometryViewWithNormals::~BufferMeshGeometryViewWithNormals() {}

void
BufferMeshGeometryViewWithNormals::bufferUpdate( const QVector< QVector3D >& pos ) {
  static_cast< BufferMeshGeometryWithNormals* >( geometry() )->updatePoints( pos );
}
