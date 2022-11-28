// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "3d/qt3dForwards.h"

#include <QObject>
#include <QVector>
#include <Qt3DCore/QGeometryView>

class BufferMeshGeometryViewWithNormals : public Qt3DCore::QGeometryView {
  Q_OBJECT

public:
  explicit BufferMeshGeometryViewWithNormals( Qt3DCore::QNode* parent = nullptr );
  ~BufferMeshGeometryViewWithNormals();

  void bufferUpdate( const QVector< QVector3D >& pos );
};
