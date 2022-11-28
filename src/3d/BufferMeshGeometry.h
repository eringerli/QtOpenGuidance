// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "3d/qt3dForwards.h"

#include <Qt3DCore/QGeometry>

class BufferMeshGeometry : public Qt3DCore::QGeometry {
  Q_OBJECT

public:
  BufferMeshGeometry( Qt3DCore::QNode* parent = nullptr );
  ~BufferMeshGeometry();

  void updatePoints( const QVector< QVector3D >& vertices );

  Qt3DCore::QAttribute* positionAttribute() const;

private:
  Qt3DCore::QAttribute* m_positionAttribute;
  Qt3DCore::QBuffer*    m_vertexBuffer;
};
