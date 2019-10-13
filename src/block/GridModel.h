// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef GRIDMODEL_H
#define GRIDMODEL_H

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLevelOfDetail>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QColor>
#include <QtMath>

#include "BlockBase.h"

#include "../kinematic/Tile.h"
#include "../kinematic/PoseOptions.h"

#include "../3d/linemesh.h"

class GridModel : public BlockBase {
    Q_OBJECT

  public:
    explicit GridModel( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity ) {
      m_distanceMeasurementEntity = new Qt3DCore::QEntity( rootEntity );
      m_distanceMeasurementTransform = new Qt3DCore::QTransform();
      m_distanceMeasurementEntity->addComponent( m_distanceMeasurementTransform );
      m_lod = new Qt3DRender::QLevelOfDetail( m_distanceMeasurementEntity );
      m_lod->setCamera( cameraEntity );
      m_distanceMeasurementEntity->addComponent( m_lod );

      m_baseEntity = new Qt3DCore::QEntity( rootEntity );

      m_baseTransform = new Qt3DCore::QTransform();
      m_baseEntity->addComponent( m_baseTransform );

      m_fineGridEntity = new Qt3DCore::QEntity( m_baseEntity );
      m_coarseGridEntity = new Qt3DCore::QEntity( m_baseEntity );

      m_fineLinesMesh = new LineMesh();
      m_fineGridEntity->addComponent( m_fineLinesMesh );

      m_coarseLinesMesh = new LineMesh();
      m_coarseGridEntity->addComponent( m_coarseLinesMesh );

      m_material = new Qt3DExtras::QPhongMaterial( m_fineGridEntity );
      m_materialCoarse = new Qt3DExtras::QPhongMaterial( m_coarseGridEntity );
      m_fineGridEntity->addComponent( m_material );
      m_coarseGridEntity->addComponent( m_materialCoarse );

      QObject::connect( m_lod, &Qt3DRender::QLevelOfDetail::currentIndexChanged, this, &GridModel::currentIndexChanged );
    }

    ~GridModel() {
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

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion, PoseOption::Options ) {
      m_distanceMeasurementEntity->setParent( tile->tileEntity );
      m_distanceMeasurementTransform->setTranslation( position );

      m_baseEntity->setParent( tile->tileEntity );

      float stepX = qMax( xStep, xStepCoarse );
      float stepY = qMax( yStep, yStepCoarse );
      QVector3D positionModulo( ( std::floor( ( tile->x + position.x() ) / stepX ) * stepX ) - tile->x,
                                ( std::floor( ( tile->y + position.y() ) / stepY ) * stepY ) - tile->y,
                                position.z() );
      m_baseTransform->setTranslation( positionModulo );
    }

    void setGrid( bool enabled ) {
      m_baseEntity->setEnabled( enabled );
    }

    void setGridValues( float xStep, float yStep, float xStepCoarse, float yStepCoarse, float size, float cameraThreshold, float cameraThresholdCoarse, QColor color, QColor colorCoarse ) {
      this->xStep = xStep;
      this->yStep = yStep;
      this->xStepCoarse = xStepCoarse;
      this->yStepCoarse = yStepCoarse;

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

        m_fineLinesMesh->posUpdate( linesPoints );
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

        m_coarseLinesMesh->posUpdate( linesPoints );
      }

      m_material->setAmbient( color );
      m_materialCoarse->setAmbient( colorCoarse );
    }

    void 	currentIndexChanged( int currentIndex ) {
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

  signals:

  private:
    Qt3DCore::QEntity* m_distanceMeasurementEntity = nullptr;
    Qt3DCore::QTransform* m_distanceMeasurementTransform = nullptr;
    Qt3DRender::QLevelOfDetail* m_lod = nullptr;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    Qt3DCore::QEntity* m_fineGridEntity = nullptr;
    Qt3DCore::QEntity* m_coarseGridEntity = nullptr;
    LineMesh* m_fineLinesMesh = nullptr;
    LineMesh* m_coarseLinesMesh = nullptr;
    Qt3DExtras::QPhongMaterial* m_material = nullptr;
    Qt3DExtras::QPhongMaterial* m_materialCoarse = nullptr;

    float xStep = 1;
    float yStep = 1;
    float xStepCoarse = 10;
    float yStepCoarse = 10;
};

class GridModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    GridModelFactory( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : BlockFactory(),
        rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Grid Model" );
    }

    virtual void addToCombobox( QComboBox* /*combobox*/ ) override {
    }

    virtual BlockBase* createNewObject() override {
      return new GridModel( rootEntity, m_cameraEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion, PoseOption::Options ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DRender::QCamera* m_cameraEntity = nullptr;
};

#endif // GRIDMODEL_H

