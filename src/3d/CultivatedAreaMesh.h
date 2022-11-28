// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <Qt3DRender/QGeometryRenderer>

#include "3d/qt3dForwards.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

class CgalThread;
class CultivatedAreaMeshGeometry;

class CultivatedAreaMesh : public Qt3DRender::QGeometryRenderer {
  Q_OBJECT

public:
  explicit CultivatedAreaMesh( Qt3DCore::QNode* parent = nullptr );
  ~CultivatedAreaMesh();

  void addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector );
  void addPointLeft( const Point_3 point, const QVector3D normalVector );
  void addPointRight( const Point_3 point, const QVector3D normalVector );

  void clear();

  void addTrackMesh( CultivatedAreaMesh* trackMesh );

  void optimise();

  size_t numPoints() const;
};
