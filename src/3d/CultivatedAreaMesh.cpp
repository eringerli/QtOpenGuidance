// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CultivatedAreaMesh.h"
#include "CultivatedAreaMeshGeometryView.h"
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>

CultivatedAreaMesh::CultivatedAreaMesh( Qt3DCore::QNode* parent ) : Qt3DRender::QGeometryRenderer( parent ) {
  auto* geometryView = new CultivatedAreaMeshGeometryView( this );
  QGeometryRenderer::setView( geometryView );
}

CultivatedAreaMesh::~CultivatedAreaMesh() { view()->setEnabled( false ); }

void
CultivatedAreaMesh::addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPoints( point1, point2, normalVector );
}

void
CultivatedAreaMesh::addPointLeft( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPointLeft( point, normalVector );
}

void
CultivatedAreaMesh::addPointRight( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addPointRight( point, normalVector );
}

void
CultivatedAreaMesh::clear() {
  //  qDebug() << "CultivatedAreaMesh::clear()";
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->clear();
}

void
CultivatedAreaMesh::addTrackMesh( CultivatedAreaMesh* trackMesh ) {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->addTrackMesh(
    static_cast< CultivatedAreaMeshGeometryView* >( trackMesh->view() ) );
}

void
CultivatedAreaMesh::optimise() {
  static_cast< CultivatedAreaMeshGeometryView* >( view() )->optimise();
}

size_t
CultivatedAreaMesh::numPoints() const {
  return static_cast< CultivatedAreaMeshGeometryView* >( view() )->numPoints();
}
