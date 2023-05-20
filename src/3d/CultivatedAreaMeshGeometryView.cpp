// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CultivatedAreaMeshGeometryView.h"
#include "CultivatedAreaMeshGeometry.h"
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QNode>
#include <Qt3DRender/QGeometryRenderer>

CultivatedAreaMeshGeometryView::CultivatedAreaMeshGeometryView( Qt3DCore::QNode* parent ) : Qt3DCore::QGeometryView( parent ) {
  auto* geometry = new CultivatedAreaMeshGeometry( this );
  QGeometryView::setGeometry( geometry );
}

CultivatedAreaMeshGeometryView::~CultivatedAreaMeshGeometryView() { geometry()->setEnabled( false ); }

void
CultivatedAreaMeshGeometryView::addPoints( const Point_3 point1, const Point_3 point2, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )->addPoints( point1, point2, normalVector );
}

void
CultivatedAreaMeshGeometryView::addPointLeft( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )->addPointLeft( point, normalVector );
}

void
CultivatedAreaMeshGeometryView::addPointRight( const Point_3 point, const QVector3D normalVector ) {
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )->addPointRight( point, normalVector );
}

void
CultivatedAreaMeshGeometryView::clear() {
  //  qDebug() << "CultivatedAreaMeshGeometryView::clear()";
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )->clear();
}

void
CultivatedAreaMeshGeometryView::addTrackMesh( CultivatedAreaMeshGeometryView* trackMesh ) {
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )
    ->addTrackMeshGeometry( static_cast< CultivatedAreaMeshGeometry* >( trackMesh->geometry() ) );
}

void
CultivatedAreaMeshGeometryView::optimise() {
  static_cast< CultivatedAreaMeshGeometry* >( geometry() )->optimise();
}

size_t
CultivatedAreaMeshGeometryView::numPoints() const {
  return static_cast< CultivatedAreaMeshGeometry* >( geometry() )->numPoints();
}
