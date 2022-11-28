// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BufferMeshGeometryWithNormals.h"
#include <QVector3D>
#include <QVector>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>

BufferMeshGeometryWithNormals::BufferMeshGeometryWithNormals( Qt3DCore::QNode* parent )
    : Qt3DCore::QGeometry( parent )
    , m_positionAttribute( new Qt3DCore::QAttribute( this ) )
    , m_normalAttribute( new Qt3DCore::QAttribute( this ) )
    , m_vertexBuffer( new Qt3DCore::QBuffer( this ) ) {
  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos*3, vec3 normal
  constexpr uint32_t elementSize = 3 + 3;
  constexpr uint32_t stride      = elementSize * sizeof( float );

  m_positionAttribute->setName( Qt3DCore::QAttribute::defaultPositionAttributeName() );
  m_positionAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
  m_positionAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );
  m_positionAttribute->setByteStride( stride );
  m_normalAttribute->setByteOffset( 0 );
  m_positionAttribute->setCount( 0 );

  m_normalAttribute->setName( Qt3DCore::QAttribute::defaultNormalAttributeName() );
  m_normalAttribute->setVertexBaseType( Qt3DCore::QAttribute::Float );
  m_normalAttribute->setVertexSize( 3 );
  m_normalAttribute->setAttributeType( Qt3DCore::QAttribute::VertexAttribute );
  m_normalAttribute->setBuffer( m_vertexBuffer );
  m_normalAttribute->setByteStride( stride );
  m_normalAttribute->setByteOffset( 3 * sizeof( float ) );
  m_normalAttribute->setCount( 0 );

  addAttribute( m_positionAttribute );
  addAttribute( m_normalAttribute );
}

BufferMeshGeometryWithNormals::~BufferMeshGeometryWithNormals() {}

Qt3DCore::QAttribute*
BufferMeshGeometryWithNormals::positionAttribute() const {
  return m_positionAttribute;
}

Qt3DCore::QAttribute*
BufferMeshGeometryWithNormals::normalAttribute() const {
  return m_normalAttribute;
}

void
BufferMeshGeometryWithNormals::updatePoints( const QVector< QVector3D >& vertices ) {
  QByteArray vertexBufferData;
  vertexBufferData.resize( vertices.size() * static_cast< int >( sizeof( QVector3D ) ) );
  memcpy( vertexBufferData.data(), vertices.constData(), static_cast< size_t >( vertexBufferData.size() ) );
  m_vertexBuffer->setData( vertexBufferData );

  m_positionAttribute->setCount( vertices.size() );
  m_normalAttribute->setCount( vertices.size() );
}
