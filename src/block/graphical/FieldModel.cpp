// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FieldModel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "CGAL/Kernel/global_functions_3.h"
#include "CGAL/enum.h"
#include "CGAL/number_utils_classes.h"
#include "block/BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "3d/BufferMesh.h"
#include "3d/BufferMeshWithNormals.h"

#include "helpers/cgalHelper.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveArc.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include <QBrush>
#include <QJsonObject>
#include <QSharedPointer>
#include <QVector>
#include <cmath>
#include <functional>
#include <utility>

FieldModel::FieldModel( Qt3DCore::QEntity* rootEntity ) {
  baseEntity    = new Qt3DCore::QEntity( rootEntity );
  baseTransform = new Qt3DCore::QTransform( baseEntity );
  baseTransform->setTranslation( QVector3D( 0, 0, 0 ) );
  baseEntity->addComponent( baseTransform );

  perimeterEntity = new Qt3DCore::QEntity( baseEntity );
  pointsEntity    = new Qt3DCore::QEntity( baseEntity );

  perimeterEntity->setEnabled( false );
  pointsEntity->setEnabled( false );

  perimeterMesh = new BufferMesh( perimeterEntity );
  perimeterMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::LineStrip );
  perimeterEntity->addComponent( perimeterMesh );

  pointsMesh = new BufferMesh( pointsEntity );
  pointsMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Points );
  pointsEntity->addComponent( pointsMesh );

  perimeterMaterial = new Qt3DExtras::QPhongMaterial( perimeterEntity );
  perimeterEntity->addComponent( perimeterMaterial );

  pointsMaterial = new Qt3DExtras::QPhongMaterial( pointsEntity );
  pointsEntity->addComponent( pointsMaterial );

  refreshColors();
}

QJsonObject
FieldModel::toJSON() {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "visible" )]        = visible;
  valuesObject[QStringLiteral( "perimeterColor" )] = perimeterColor.name();
  valuesObject[QStringLiteral( "pointsColor" )]    = pointsColor.name();

  return valuesObject;
}

void
FieldModel::fromJSON( QJsonObject& valuesObject ) {
  visible        = valuesObject[QStringLiteral( "visible" )].toBool( true );
  perimeterColor = QColor( valuesObject[QStringLiteral( "perimeterColor" )].toString( QStringLiteral( "#00ff00" ) ) );
  pointsColor    = QColor( valuesObject[QStringLiteral( "pointsColor" )].toString( QStringLiteral( "#0000ff" ) ) );

  refreshColors();
}

void
FieldModel::refreshColors() {
  perimeterMaterial->setAmbient( perimeterColor );
  pointsMaterial->setAmbient( pointsColor );
}

void
FieldModel::setVisible( const bool visible ) {
  this->visible = visible;
  baseEntity->setEnabled( visible );
}

void
FieldModel::setPoints( const std::vector< Epick::Point_3 >& pointsVector ) {
  points.clear();

  for( const auto& point : pointsVector ) {
    points.push_back( toQVector3D( point ) );
  }

  pointsMesh->bufferUpdate( points );
  pointsEntity->setEnabled( true );
}

void
FieldModel::addPoint( const Eigen::Vector3d& point ) {
  points.push_back( toQVector3D( point ) );

  pointsMesh->bufferUpdate( points );
  pointsEntity->setEnabled( true );
}

void
FieldModel::clearPoints() {
  points.clear();
  pointsEntity->setEnabled( false );
}

void
FieldModel::setField( std::shared_ptr< Polygon_with_holes_2 > field ) {
  QVector< QVector3D > meshSegmentPoints;

  for( const auto& vi : field->outer_boundary() ) {
    meshSegmentPoints << toQVector3D( vi, 0.1f );
  }

  meshSegmentPoints.push_back( meshSegmentPoints.first() );
  perimeterMesh->bufferUpdate( meshSegmentPoints );

  perimeterEntity->setEnabled( true );
}

void
FieldModel::clearField() {
  perimeterEntity->setEnabled( false );
}
