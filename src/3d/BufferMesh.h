// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QVector>
#include <Qt3DRender/QGeometryRenderer>

#include "3d/qt3dForwards.h"

class BufferMesh : public Qt3DRender::QGeometryRenderer {
  Q_OBJECT

public:
  explicit BufferMesh( Qt3DCore::QNode* parent = nullptr );
  ~BufferMesh();

public Q_SLOTS:
  void bufferUpdate( const QVector< QVector3D >& pos );
};
