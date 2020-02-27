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

#include <QVector>
#include <QVector3D>
#include "BufferMeshGeometry.h"

BufferMeshGeometry::BufferMeshGeometry( Qt3DCore::QNode* parent ) :
  Qt3DRender::QGeometry( parent )
  , m_positionAttribute( new Qt3DRender::QAttribute( this ) )
  , m_vertexBuffer( new Qt3DRender::QBuffer( this ) ) {
  m_positionAttribute->setAttributeType( Qt3DRender::QAttribute::VertexAttribute );
  m_positionAttribute->setBuffer( m_vertexBuffer );

#if QT_VERSION >= 0x050800
  m_positionAttribute->setVertexBaseType( Qt3DRender::QAttribute::Float );
  m_positionAttribute->setVertexSize( 3 );
#else
  m_positionAttribute->setDataType( Qt3DRender::QAttribute::Float );
  m_positionAttribute->setDataSize( 3 );
#endif

  m_positionAttribute->setName( Qt3DRender::QAttribute::defaultPositionAttributeName() );

  addAttribute( m_positionAttribute );
}

BufferMeshGeometry::~BufferMeshGeometry() {
  m_positionAttribute->deleteLater();
  m_vertexBuffer->deleteLater();
}

int BufferMeshGeometry::vertexCount() {
  return m_vertexBuffer->data().size() / static_cast<int>( sizeof( QVector3D ) );
}

void BufferMeshGeometry::updatePoints( const QVector<QVector3D>& vertices ) {
  QByteArray vertexBufferData;
  vertexBufferData.resize( vertices.size() * static_cast<int>( sizeof( QVector3D ) ) );
  memcpy( vertexBufferData.data(), vertices.constData(), static_cast<size_t>( vertexBufferData.size() ) );
  m_vertexBuffer->setData( vertexBufferData );
}

#include "moc_BufferMeshGeometry.cpp"
