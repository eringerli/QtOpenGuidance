// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TerrainModel.h"

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
#include <Qt3DExtras/QMetalRoughMaterial>
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

#include <CGAL/Point_set_3.h>
using PointSet_3 = CGAL::Point_set_3< Point_3 >;

#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Projection_traits_xy_3.h>
using ProjectionTraitsXY               = CGAL::Projection_traits_xy_3< Epick >;
using DelaunayTriangulationProjectedXY = CGAL::Delaunay_triangulation_2< ProjectionTraitsXY >;

#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Surface_mesh.h>
using SurfaceMesh_3 = CGAL::Surface_mesh< Point_3 >;

#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>

// #include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
namespace PMP           = CGAL::Polygon_mesh_processing;
using vertex_descriptor = boost::graph_traits< SurfaceMesh_3 >::vertex_descriptor;
using face_descriptor   = boost::graph_traits< SurfaceMesh_3 >::face_descriptor;

#include <CGAL/Polygon_mesh_processing/locate.h>
#include <CGAL/Surface_mesh.h>
using SurfaceMesh_3 = CGAL::Surface_mesh< Point_3 >;

Q_DECLARE_METATYPE( std::shared_ptr< SurfaceMesh_3 > )

TerrainModel::TerrainModel( Qt3DCore::QEntity* rootEntity, bool usePBR ) : usePBR( usePBR ) {
  baseEntity    = new Qt3DCore::QEntity( rootEntity );
  baseTransform = new Qt3DCore::QTransform( baseEntity );
  baseTransform->setTranslation( QVector3D( 0, 0, 0 ) );
  baseEntity->addComponent( baseTransform );

  linesEntity   = new Qt3DCore::QEntity( baseEntity );
  terrainEntity = new Qt3DCore::QEntity( baseEntity );

  linesEntity->setEnabled( false );
  terrainEntity->setEnabled( false );

  linesMesh = new BufferMesh( linesEntity );
  linesMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  linesEntity->addComponent( linesMesh );

  terrainMesh = new BufferMeshWithNormals( terrainEntity );
  terrainMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Triangles );
  terrainEntity->addComponent( terrainMesh );

  if( usePBR ) {
    terrainMaterialPbr = new Qt3DExtras::QMetalRoughMaterial( terrainEntity );
    linesMaterialPbr   = new Qt3DExtras::QMetalRoughMaterial( linesEntity );
    terrainEntity->addComponent( terrainMaterialPbr );
    linesEntity->addComponent( linesMaterialPbr );
  } else {
    terrainMaterial = new Qt3DExtras::QPhongMaterial( terrainEntity );
    linesMaterial   = new Qt3DExtras::QPhongMaterial( linesEntity );
    terrainEntity->addComponent( terrainMaterial );
    linesEntity->addComponent( linesMaterial );
  }

  refreshColors();
}

QJsonObject
TerrainModel::toJSON() {
  QJsonObject valuesObject;

  valuesObject[QStringLiteral( "visible" )]      = visible;
  valuesObject[QStringLiteral( "terrainColor" )] = terrainColor.name();
  valuesObject[QStringLiteral( "lineColor" )]    = linesColor.name();

  return valuesObject;
}

void
TerrainModel::fromJSON( QJsonObject& valuesObject ) {
  visible      = valuesObject[QStringLiteral( "visible" )].toBool( true );
  terrainColor = QColor( valuesObject[QStringLiteral( "terrainColor" )].toString( QStringLiteral( "#00aa00" ) ) );
  linesColor   = QColor( valuesObject[QStringLiteral( "lineColor" )].toString( QStringLiteral( "#00ff00" ) ) );

  refreshColors();
}

void
TerrainModel::refreshColors() {
  if( usePBR ) {
    linesMaterialPbr->setBaseColor( linesColor );
    linesMaterialPbr->setMetalness( 0.0f );
    linesMaterialPbr->setRoughness( 0.5f );

    terrainMaterialPbr->setBaseColor( terrainColor );
    terrainMaterialPbr->setMetalness( 0.0f );
    terrainMaterialPbr->setRoughness( 0.5f );
  } else {
    linesMaterial->setShininess( 0.05f );
    terrainMaterial->setShininess( 0.05f );
    linesMaterial->setAmbient( linesColor );
    terrainMaterial->setAmbient( terrainColor );
  }
}

void
TerrainModel::setVisible( const bool visible ) {
  this->visible = visible;
  baseEntity->setEnabled( visible );
}

void
TerrainModel::setSurface( std::shared_ptr< SurfaceMesh_3 > surfaceMesh ) {
  auto vnormals = surfaceMesh->add_property_map< vertex_descriptor, Vector_3 >( "v:normals", CGAL::NULL_VECTOR ).first;
  auto fnormals = surfaceMesh->add_property_map< face_descriptor, Vector_3 >( "f:normals", CGAL::NULL_VECTOR ).first;

  PMP::compute_normals( *surfaceMesh, vnormals, fnormals );

  Eigen::IOFormat CommaInitFmt( Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "(", ")" );

  auto ppm = get( CGAL::vertex_point, *surfaceMesh );

  QVector< QVector3D > trianglesWithNormals;

  for( face_descriptor fd : faces( *surfaceMesh ) ) {
    auto       normal       = fnormals[fd];
    const auto normalVector = toQVector3D( normal );

    for( vertex_descriptor vd : CGAL::vertices_around_face( surfaceMesh->halfedge( fd ), *surfaceMesh ) ) {
      trianglesWithNormals << toQVector3D( get( ppm, vd ) );
      trianglesWithNormals << normalVector;
    }

    auto quaternion = Eigen::Quaterniond::FromTwoVectors( Eigen::Vector3d( 0, 0, 1 ), toEigenVector( normal ) );
  }

  terrainMesh->bufferUpdate( trianglesWithNormals );
  terrainEntity->setEnabled( true );

  QVector< QVector3D > lines;

  for( const auto& edge : surfaceMesh->edges() ) {
    auto point1 = surfaceMesh->point( surfaceMesh->vertex( edge, 0 ) );
    auto point2 = surfaceMesh->point( surfaceMesh->vertex( edge, 1 ) );

    lines << toQVector3D( point1 );
    lines << toQVector3D( point2 );
  }

  linesMesh->bufferUpdate( lines );
  linesEntity->setEnabled( true );
}
