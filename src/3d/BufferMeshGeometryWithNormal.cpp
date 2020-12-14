/* Atelier KDE Printer Host for 3D Printing
    Copyright (C) <2017-2018>
    Author: Patrick José Pereira - patrickjp@kde.org
            Kevin Ottens - ervin@kde.org
            Christian Riggenbach

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of
    the License or any later version accepted by the membership of
    KDE e.V. (or its successor approved by the membership of KDE
    e.V.), which shall act as a proxy defined in Section 14 of
    version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BufferMeshGeometryWithNormal.h"
#include <QVector>
#include <QVector3D>

BufferMeshGeometryWithNormal::BufferMeshGeometryWithNormal( Qt3DCore::QNode* parent ) :
  Qt3DRender::QGeometry( parent )
  , m_positionAttribute( new Qt3DRender::QAttribute( this ) )
  , m_normalAttribute( new Qt3DRender::QAttribute( this ) )
  , m_vertexBuffer( new Qt3DRender::QBuffer( this ) ) {

  // Populate a buffer with the interleaved per-vertex data with
  // vec3 pos*3, vec3 normal
  constexpr uint32_t elementSize = 3 + 3;
  constexpr uint32_t stride = elementSize * sizeof( float );

  m_positionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );
  m_positionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
  m_positionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );
  m_positionAttribute->setByteStride( stride );
  m_positionAttribute->setCount( 0 );

  m_normalAttribute->setName( Qt3DRender::QAttribute::defaultNormalAttributeName() );
  m_normalAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_normalAttribute->setVertexSize( 3 );
  m_normalAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_normalAttribute->setBuffer( m_vertexBuffer );
  m_normalAttribute->setByteStride( stride );
  m_normalAttribute->setByteOffset( 3 * sizeof( float ) );
  m_normalAttribute->setCount( 0 );

  addAttribute( m_positionAttribute );
  addAttribute( m_normalAttribute );
}

BufferMeshGeometryWithNormal::~BufferMeshGeometryWithNormal() {
  m_positionAttribute->deleteLater();
  m_normalAttribute->deleteLater();
  m_vertexBuffer->deleteLater();
}

int BufferMeshGeometryWithNormal::vertexCount() {
  return m_vertexBuffer->data().size() / static_cast<int>( sizeof( QVector3D ) );
}

void BufferMeshGeometryWithNormal::updatePoints( const QVector<QVector3D>& vertices ) {
  QByteArray vertexBufferData;
  vertexBufferData.resize( vertices.size() * static_cast<int>( sizeof( QVector3D ) ) );
  memcpy( vertexBufferData.data(), vertices.constData(), static_cast<size_t>( vertexBufferData.size() ) );
  m_vertexBuffer->setData( vertexBufferData );
}
