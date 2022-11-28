// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMeshGeometryView.h"
#include "BufferMeshGeometry.h"
#include <QVector3D>
#include <QVector4D>

BufferMeshGeometryView::BufferMeshGeometryView( Qt3DCore::QNode* parent ) : Qt3DCore::QGeometryView( parent ) {
  auto* geometry = new BufferMeshGeometry( this );
  QGeometryView::setGeometry( geometry );
}

BufferMeshGeometryView::~BufferMeshGeometryView() {}

void
BufferMeshGeometryView::bufferUpdate( const QVector< QVector3D >& pos ) {
  static_cast< BufferMeshGeometry* >( geometry() )->updatePoints( pos );
}
