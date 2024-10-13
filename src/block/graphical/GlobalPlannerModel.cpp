// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GlobalPlannerModel.h"

#include <QObject>

#include <QQuaternion>
#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>

#include <QDebug>

#include "CGAL/Kernel/global_functions_3.h"
#include "CGAL/enum.h"
#include "CGAL/number_utils_classes.h"
#include "block/BlockBase.h"

#include "helpers/cgalHelper.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/Plan.h"

#include "kinematic/PathPrimitive.h"
#include "kinematic/PathPrimitiveArc.h"
#include "kinematic/PathPrimitiveLine.h"
#include "kinematic/PathPrimitiveRay.h"
#include "kinematic/PathPrimitiveSegment.h"
#include "kinematic/PathPrimitiveSequence.h"

#include <QJsonObject>
#include <QSharedPointer>
#include <QVector>
#include <cmath>
#include <functional>
#include <utility>

GlobalPlannerModel::GlobalPlannerModel( Qt3DCore::QEntity* rootEntity, const BlockBaseId idHint, const bool systemBlock, const QString type )
    : BlockBase( idHint, systemBlock, type ) {
  baseEntity    = new Qt3DCore::QEntity( rootEntity );
  baseTransform = new Qt3DCore::QTransform( baseEntity );
  baseEntity->addComponent( baseTransform );

  // a point marker -> orange
  {
    aPointEntity = new Qt3DCore::QEntity( baseEntity );

    aPointMesh = new Qt3DExtras::QSphereMesh( aPointEntity );
    aPointMesh->setRadius( sizeOfPoint );
    aPointMesh->setSlices( 20 );
    aPointMesh->setRings( 20 );

    aPointTransform = new Qt3DCore::QTransform( aPointEntity );

    aMaterial = new Qt3DExtras::QPhongMaterial( aPointEntity );

    aPointEntity->addComponent( aPointMesh );
    aPointEntity->addComponent( aMaterial );
    aPointEntity->addComponent( aPointTransform );
    aPointEntity->setEnabled( false );

    aTextEntity     = new Qt3DCore::QEntity( aPointEntity );
    auto* aTextMesh = new Qt3DExtras::QExtrudedTextMesh( aTextEntity );
    aTextMesh->setText( QStringLiteral( "A" ) );
    aTextMesh->setDepth( 0.05f );

    aTextEntity->setEnabled( true );
    aTextTransform = new Qt3DCore::QTransform( aTextEntity );
    aTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    aTextTransform->setScale( 2.0f );
    aTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    aTextEntity->addComponent( aTextTransform );
    aTextEntity->addComponent( aTextMesh );
    aTextEntity->addComponent( aMaterial );
  }

  // b point marker -> purple
  {
    bPointEntity = new Qt3DCore::QEntity( baseEntity );
    bPointMesh   = new Qt3DExtras::QSphereMesh( bPointEntity );
    bPointMesh->setRadius( sizeOfPoint );
    bPointMesh->setSlices( 20 );
    bPointMesh->setRings( 20 );

    bPointTransform = new Qt3DCore::QTransform( bPointEntity );

    bMaterial = new Qt3DExtras::QPhongMaterial( aPointEntity );

    bPointEntity->addComponent( bPointMesh );
    bPointEntity->addComponent( bMaterial );
    bPointEntity->addComponent( bPointTransform );
    bPointEntity->setEnabled( false );

    bTextEntity     = new Qt3DCore::QEntity( bPointEntity );
    auto* bTextMesh = new Qt3DExtras::QExtrudedTextMesh( bTextEntity );
    bTextMesh->setText( QStringLiteral( "B" ) );
    bTextMesh->setDepth( 0.05f );

    bTextEntity->setEnabled( true );
    bTextTransform = new Qt3DCore::QTransform( bTextEntity );
    bTextTransform->setRotation( QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -90 ) );
    bTextTransform->setScale( 2.0f );
    bTextTransform->setTranslation( QVector3D( 0, -.2f, 0 ) );
    bTextEntity->addComponent( bTextTransform );
    bTextEntity->addComponent( bTextMesh );
    bTextEntity->addComponent( bMaterial );
  }

  {
    pointsEntity = new Qt3DCore::QEntity( baseEntity );

    pointsMesh = new Qt3DExtras::QSphereMesh( pointsEntity );
    pointsMesh->setRadius( sizeOfPoint );
    pointsMesh->setSlices( 20 );
    pointsMesh->setRings( 20 );

    pointsMaterial = new Qt3DExtras::QPhongMaterial( pointsEntity );
  }

  refreshColors();
}

void
GlobalPlannerModel::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "visible" )]     = visible;
  valuesObject[QStringLiteral( "sizeOfPoint" )] = sizeOfPoint;
  valuesObject[QStringLiteral( "aColor" )]      = aColor.name();
  valuesObject[QStringLiteral( "bColor" )]      = bColor.name();
  valuesObject[QStringLiteral( "pointColor" )]  = pointColor.name();
}

void
GlobalPlannerModel::fromJSON( const QJsonObject& valuesObject ) {
  visible     = valuesObject[QStringLiteral( "visible" )].toBool( true );
  sizeOfPoint = valuesObject[QStringLiteral( "sizeOfPoint" )].toDouble( 1 );
  aColor      = QColor( valuesObject[QStringLiteral( "aColor" )].toString( QStringLiteral( "#ffa500" ) ) );
  bColor      = QColor( valuesObject[QStringLiteral( "bColor" )].toString( QStringLiteral( "#ffa500" ) ) );
  pointColor  = QColor( valuesObject[QStringLiteral( "pointColor" )].toString( QStringLiteral( "#800080" ) ) );

  refreshColors();
  setSizeOfPoint( sizeOfPoint );
}

void
GlobalPlannerModel::refreshColors() {
  aMaterial->setDiffuse( aColor );
  bMaterial->setDiffuse( bColor );
  pointsMaterial->setDiffuse( pointColor );
}

void
GlobalPlannerModel::setVisible( const bool visible ) {
  this->visible = visible;
  baseEntity->setEnabled( visible );
}

void
GlobalPlannerModel::enable( const bool enable ) {
  BlockBase::enable( enable );

  setVisible( enable );
}

void
GlobalPlannerModel::setSizeOfPoint( const float sizeOfPoint ) {
  aPointMesh->setRadius( sizeOfPoint );
  bPointMesh->setRadius( sizeOfPoint );
  pointsMesh->setRadius( sizeOfPoint );

  this->sizeOfPoint = sizeOfPoint;
}

void
GlobalPlannerModel::setPose( const Eigen::Vector3d&           position,
                             const Eigen::Quaterniond&        orientation,
                             const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::Graphical ) ) {
    auto quat = toQQuaternion( orientation );
    aPointTransform->setRotation( quat );
    bPointTransform->setRotation( quat );
  }
}

void
GlobalPlannerModel::showPlanPolyline( std::shared_ptr< std::vector< Point_2 > > polylinePtr ) {
  if( polylinePtr->size() == 1 ) {
    aPointTransform->setTranslation( toQVector3D( polylinePtr->front() ) );

    aPointEntity->setEnabled( true );
    bPointEntity->setEnabled( false );
    pointsEntity->setEnabled( false );
  } else if( polylinePtr->size() == 2 ) {
    aPointTransform->setTranslation( toQVector3D( polylinePtr->front() ) );
    bPointTransform->setTranslation( toQVector3D( polylinePtr->back() ) );

    aPointEntity->setEnabled( true );
    bPointEntity->setEnabled( true );
    pointsEntity->setEnabled( false );
  } else if( polylinePtr->size() > 2 ) {
    aPointTransform->setTranslation( toQVector3D( polylinePtr->front() ) );
    bPointTransform->setTranslation( toQVector3D( polylinePtr->back() ) );

    for( const auto& child : std::as_const( pointsEntity->children() ) ) {
      if( auto* childPtr = qobject_cast< Qt3DCore::QEntity* >( child ) ) {
        childPtr->setEnabled( false );
        childPtr->deleteLater();
      }
    }

    for( auto it = polylinePtr->cbegin() + 1, end = polylinePtr->cend() - 1; it != end; ++it ) {
      auto* entity = new Qt3DCore::QEntity( pointsEntity );

      auto* transform = new Qt3DCore::QTransform( entity );
      transform->setTranslation( toQVector3D( *it ) );

      entity->addComponent( pointsMaterial );
      entity->addComponent( pointsMesh );
      entity->addComponent( transform );

      entity->setEnabled( true );
    }

    aPointEntity->setEnabled( true );
    bPointEntity->setEnabled( true );
    pointsEntity->setEnabled( true );
  }
}
