// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "3d/qt3dForwards.h"

#include <QObject>
#include <QVector>
#include <Qt3DRender/QGeometryRenderer>

class QString;

class BufferMeshWithNormals : public Qt3DRender::QGeometryRenderer {
  Q_OBJECT

public:
  explicit BufferMeshWithNormals( Qt3DCore::QNode* parent = nullptr );
  ~BufferMeshWithNormals();

  void bufferUpdate( const QVector< QVector3D >& pos );
};
