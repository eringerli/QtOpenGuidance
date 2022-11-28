// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
