// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QVector>
#include <Qt3DCore/QGeometryView>

#include "3d/qt3dForwards.h"

class BufferMeshGeometryView : public Qt3DCore::QGeometryView {
  Q_OBJECT

public:
  explicit BufferMeshGeometryView( Qt3DCore::QNode* parent = nullptr );
  ~BufferMeshGeometryView();

  void bufferUpdate( const QVector< QVector3D >& pos );
};
