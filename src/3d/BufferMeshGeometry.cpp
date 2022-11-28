// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMeshGeometry.h"
#include <QVector3D>
#include <QVector>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>

BufferMeshGeometry::BufferMeshGeometry( Qt3DCore::QNode* parent )
    : Qt3DCore::QGeometry( parent )
    , m_positionAttribute( new Qt3DCore::QAttribute( this ) )
    , m_vertexBuffer( new Qt3DCore::QBuffer( this ) ) {
  m_positionAttribute->setName( Qt3DCore::QAttribute::defaultPositionAttributeName() );
  m_positionAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );

  m_positionAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
  m_positionAttribute->setByteStride( 3 * sizeof( float ) );

  m_positionAttribute->setCount( 0 );

  addAttribute( m_positionAttribute );
}

BufferMeshGeometry::~BufferMeshGeometry() {}

Qt3DCore::QAttribute*
BufferMeshGeometry::positionAttribute() const {
  return m_positionAttribute;
}

void
BufferMeshGeometry::updatePoints( const QVector< QVector3D >& vertices ) {
  QByteArray vertexBufferData;
  vertexBufferData.resize( vertices.size() * static_cast< int >( sizeof( QVector3D ) ) );
  memcpy( vertexBufferData.data(), vertices.constData(), static_cast< size_t >( vertexBufferData.size() ) );
  m_vertexBuffer->setData( vertexBufferData );

  m_positionAttribute->setCount( vertices.size() );
}
