// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <Qt3DCore/QGeometryView>

#include "3d/qt3dForwards.h"

#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"

class CgalThread;

class CultivatedAreaMesh;
class CultivatedAreaMeshGeometry;

class CultivatedAreaMeshGeometryView : public Qt3DCore::QGeometryView {
  Q_OBJECT

public:
  explicit CultivatedAreaMeshGeometryView( Qt3DCore::QNode* parent = nullptr );
  ~CultivatedAreaMeshGeometryView();

  void addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector );
  void addPointLeft( const Point_3 point, const QVector3D normalVector );
  void addPointRight( const Point_3 point, const QVector3D normalVector );

  void clear();

  void addTrackMesh( CultivatedAreaMeshGeometryView* trackMesh );

  void optimise();

  size_t numPoints() const;
};
