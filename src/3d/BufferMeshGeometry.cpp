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
