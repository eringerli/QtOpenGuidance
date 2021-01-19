// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

#include "GridModel.h"

#include "qneblock.h"
#include "qneport.h"

GridModel::GridModel( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity ) {
  m_distanceMeasurementEntity = new Qt3DCore::QEntity( rootEntity );
  m_distanceMeasurementTransform = new Qt3DCore::QTransform( m_distanceMeasurementEntity );
  m_distanceMeasurementEntity->addComponent( m_distanceMeasurementTransform );
  m_lod = new Qt3DRender::QLevelOfDetail( m_distanceMeasurementEntity );
  m_lod->setCamera( cameraEntity );
  m_distanceMeasurementEntity->addComponent( m_lod );

  m_baseEntity = new Qt3DCore::QEntity( rootEntity );

  m_baseTransform = new Qt3DCore::QTransform( m_baseEntity );
  m_baseEntity->addComponent( m_baseTransform );

  m_fineGridEntity = new Qt3DCore::QEntity( m_baseEntity );
  m_coarseGridEntity = new Qt3DCore::QEntity( m_baseEntity );

  m_fineLinesMesh = new BufferMesh( m_fineGridEntity );
  m_fineGridEntity->addComponent( m_fineLinesMesh );

  m_coarseLinesMesh = new BufferMesh( m_coarseGridEntity );
  m_coarseGridEntity->addComponent( m_coarseLinesMesh );

  m_material = new Qt3DExtras::QPhongMaterial( m_fineGridEntity );
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

void GridModel::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& ) {
  m_distanceMeasurementTransform->setTranslation( toQVector3D( position ) );

  QVector3D positionModulo( float( std::floor( ( position.x() ) / xStepMax ) * xStepMax ),
                            float( std::floor( ( position.y() ) / yStepMax ) * yStepMax ),
                            float( position.z() ) );
  m_baseTransform->setTranslation( positionModulo );

  auto taitBryan = quaternionToTaitBryan( orientation );
  m_baseTransform->setRotation( toQQuaternion( orientation * taitBryanToQuaternion( getYaw( taitBryan ), 0, 0 ).conjugate() ) );
}

void GridModel::setGrid( bool enabled ) {
  m_baseEntity->setEnabled( enabled );
}

void GridModel::setGridValues( const float xStep, const float yStep, const float xStepCoarse, const float yStepCoarse, const float size, const float cameraThreshold, const float cameraThresholdCoarse, const QColor color, const QColor colorCoarse ) {
  this->xStep = double( xStep );
  this->yStep = double( yStep );
  this->xStepCoarse = double( xStepCoarse );
  this->yStepCoarse = double( yStepCoarse );
  this->xStepMax = double( std::max( xStep, xStepCoarse ) );
  this->yStepMax = double( std::max( yStep, yStepCoarse ) );

  QVector<qreal> thresholds = {qreal( cameraThreshold ), qreal( cameraThresholdCoarse ), 10000};
  m_lod->setThresholds( thresholds );

  // fine
  {
    QVector<QVector3D> linesPoints;

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
    QVector<QVector3D> linesPoints;

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

void GridModel::currentIndexChanged( const int currentIndex ) {
  switch( currentIndex ) {
    case 0: {
      m_fineGridEntity->setEnabled( true );
      m_coarseGridEntity->setEnabled( true );
    }
    break;

    case 1: {
      m_fineGridEntity->setEnabled( false );
      m_coarseGridEntity->setEnabled( true );
    }
    break;

    default: {
      m_fineGridEntity->setEnabled( false );
      m_coarseGridEntity->setEnabled( false );
    }
  }

}

QNEBlock* GridModelFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new GridModel( rootEntity, m_cameraEntity );
  auto* b = createBaseBlock( scene, obj, id, true );

  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  return b;
}
