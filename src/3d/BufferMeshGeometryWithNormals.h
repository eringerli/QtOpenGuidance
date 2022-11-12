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

#pragma once

#include "3d/qt3dForwards.h"

#include <Qt3DCore/QGeometry>

class BufferMeshGeometryWithNormals : public Qt3DCore::QGeometry {
  Q_OBJECT

public:
  BufferMeshGeometryWithNormals( Qt3DCore::QNode* parent = nullptr );
  ~BufferMeshGeometryWithNormals();

  void updatePoints( const QVector< QVector3D >& vertices );

  Qt3DCore::QAttribute* positionAttribute() const;
  Qt3DCore::QAttribute* normalAttribute() const;

private:
  Qt3DCore::QAttribute* m_positionAttribute = nullptr;
  Qt3DCore::QAttribute* m_normalAttribute   = nullptr;
  Qt3DCore::QBuffer*    m_vertexBuffer      = nullptr;
};
