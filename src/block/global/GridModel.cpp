// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "GridModel.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/eigenHelper.h"

GridModel::GridModel( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity ) {
  m_distanceMeasurementEntity    = new Qt3DCore::QEntity( rootEntity );
  m_distanceMeasurementTransform = new Qt3DCore::QTransform( m_distanceMeasurementEntity );
  m_distanceMeasurementEntity->addComponent( m_distanceMeasurementTransform );
  m_lod = new Qt3DRender::QLevelOfDetail( m_distanceMeasurementEntity );
  m_lod->setCamera( cameraEntity );
  m_distanceMeasurementEntity->addComponent( m_lod );

  m_baseEntity = new Qt3DCore::QEntity( rootEntity );

  m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
  m_baseEntity->addComponent( m_baseTransform );

  m_fineGridEntity   = new Qt3DCore::QEntity( m_baseEntity );
  m_coarseGridEntity = new Qt3DCore::QEntity( m_baseEntity );

  m_fineLinesMesh = new BufferMesh( m_fineGridEntity );
  m_fineLinesMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  m_fineGridEntity->addComponent( m_fineLinesMesh );

  m_coarseLinesMesh = new BufferMesh( m_coarseGridEntity );
  m_coarseLinesMesh->view()->setPrimitiveType( Qt3DCore::QGeometryView::Lines );
  m_coarseGridEntity->addComponent( m_coarseLinesMesh );

  m_material       = new Qt3DExtras::QPhongMaterial( m_fineGridEntity );
  m_materialCoarse = new Qt3DExtras::QPhongMaterial( m_coarseGridEntity );
  m_fineGridEntity->addComponent( m_material );
  m_coarseGridEntity->addComponent( m_materialCoarse );

  QObject::connect( m_lod, &Qt3DRender::QLevelOfDetail::currentIndexChanged, this, &GridModel::currentIndexChanged );
}

GridModel::~GridModel() {
  m_fineLinesMesh->setEnabled( false );
  m_coarseLinesMesh->setEnabled( false );
  m_material->setEnabled( false );
  m_fineGridEntity->setEnabled( false );
  m_coarseGridEntity->setEnabled( false );
  m_baseTransform->setEnabled( false );
  m_baseEntity->setEnabled( false );
  m_lod->setEnabled( false );
  m_distanceMeasurementTransform->setEnabled( false );
  m_distanceMeasurementEntity->setEnabled( false );

  m_fineLinesMesh->deleteLater();
  m_coarseLinesMesh->deleteLater();
  m_material->deleteLater();
  m_fineGridEntity->deleteLater();
  m_coarseGridEntity->deleteLater();
  m_baseTransform->deleteLater();
  m_baseEntity->deleteLater();
  m_lod->deleteLater();
  m_distanceMeasurementTransform->deleteLater();
  m_distanceMeasurementEntity->deleteLater();
}

void
GridModel::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::Option::NoGraphics ) ) {
    m_distanceMeasurementTransform->setTranslation( toQVector3D( position ) );

    auto offset =
      Eigen::Vector3d( std::floor( ( position.x() ) / xStepMax ) * xStepMax, std::floor( ( position.y() ) / yStepMax ) * yStepMax, 0 );

    auto absoluteOrientationOfPlane = getAbsoluteOrientation( orientation );

    Eigen::Vector3d offsetOfGridOriginFromPosition = offset - position;
    offsetOfGridOriginFromPosition.z()             = 0;

    m_baseTransform->setTranslation(
      QVector3D( offset.x(),
                 offset.y(),
                 FixedKinematicPrimitive::calculate( position, offsetOfGridOriginFromPosition, absoluteOrientationOfPlane ).z() ) );

    m_baseTransform->setRotation( toQQuaternion( absoluteOrientationOfPlane ) );
  }
}

void
GridModel::setGrid( bool enabled ) {
  m_baseEntity->setEnabled( enabled );
}

void
GridModel::setGridValues( const float  xStep,
                          const float  yStep,
                          const float  xStepCoarse,
                          const float  yStepCoarse,
                          const float  size,
                          const float  cameraThreshold,
                          const float  cameraThresholdCoarse,
                          const QColor color,
                          const QColor colorCoarse ) {
  this->xStep       = double( xStep );
  this->yStep       = double( yStep );
  this->xStepCoarse = double( xStepCoarse );
  this->yStepCoarse = double( yStepCoarse );
  this->xStepMax    = double( std::max( xStep, xStepCoarse ) );
  this->yStepMax    = double( std::max( yStep, yStepCoarse ) );

  QVector< qreal > thresholds = { qreal( cameraThreshold ), qreal( cameraThresholdCoarse ), 10000 };
  m_lod->setThresholds( thresholds );

  // fine
  {
    QVector< QVector3D > linesPoints;

    // Lines in X direction
    {
      QVector3D start( -size / 2, 0, -0.05f );
      QVector3D end( size / 2, 0, -0.05f );

      for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += yStep ) {
        start.setY( lineDistance );
        end.setY( lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );

        start.setY( -lineDistance );
        end.setY( -lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );
      }
    }

    // Lines in Y direction
    {
      QVector3D start( 0, -size / 2, -0.05f );
      QVector3D end( 0, size / 2, -0.05f );

      for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += xStep ) {
        start.setX( lineDistance );
        end.setX( lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );

        start.setX( -lineDistance );
        end.setX( -lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );
      }
    }

    m_fineLinesMesh->bufferUpdate( linesPoints );
  }

  // coarse
  {
    QVector< QVector3D > linesPoints;

    // Lines in X direction
    {
      QVector3D start( -size / 2, 0, -0.001f );
      QVector3D end( size / 2, 0, -0.001f );

      for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += yStepCoarse ) {
        start.setY( lineDistance );
        end.setY( lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );

        start.setY( -lineDistance );
        end.setY( -lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );
      }
    }

    // Lines in Y direction
    {
      QVector3D start( 0, -size / 2, -0.001f );
      QVector3D end( 0, size / 2, -0.001f );

      for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += xStepCoarse ) {
        start.setX( lineDistance );
        end.setX( lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );

        start.setX( -lineDistance );
        end.setX( -lineDistance );
        linesPoints.append( start );
        linesPoints.append( end );
      }
    }

    m_coarseLinesMesh->bufferUpdate( linesPoints );
  }

  m_material->setAmbient( color );
  m_materialCoarse->setAmbient( colorCoarse );
}

void
GridModel::currentIndexChanged( const int currentIndex ) {
  switch( currentIndex ) {
    case 0: {
      m_fineGridEntity->setEnabled( true );
      m_coarseGridEntity->setEnabled( true );
    } break;

    case 1: {
      m_fineGridEntity->setEnabled( false );
      m_coarseGridEntity->setEnabled( true );
    } break;

    default: {
      m_fineGridEntity->setEnabled( false );
      m_coarseGridEntity->setEnabled( false );
    }
  }
}

QNEBlock*
GridModelFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new GridModel( rootEntity, m_cameraEntity );
  auto* b   = createBaseBlock( scene, obj, id, true );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  return b;
}
