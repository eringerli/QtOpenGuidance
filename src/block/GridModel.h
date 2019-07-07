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

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>

#include <QColor>
#include <QtMath>

#include "GuidanceBase.h"

#include "../kinematic/Tile.h"

#include "../3d/drawline.h"

class GridModel : public GuidanceBase {
    Q_OBJECT

  public:
    explicit GridModel( Qt3DCore::QEntity* rootEntity ) {
      m_rootEntity = rootEntity;
    }
    ~GridModel() {
      m_baseEntity->deleteLater();
      m_baseTransform->deleteLater();
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion ) {
      if( m_baseEntity != nullptr ) {
        m_baseEntity->setParent( tile->tileEntity );
        QVector3D positionModulo( std::floor( position.x() / xStep )*xStep,
                                  std::floor( position.y() / yStep )*yStep,
                                  position.z() );
        m_baseTransform->setTranslation( positionModulo );
      }
    }

    void setGrid( bool enabled ) {
      gridEnabled = enabled;

      if( m_baseEntity != nullptr ) {
        m_baseEntity->setEnabled( gridEnabled );
      }
    }

    void setGridValues( float xStep, float yStep, float size, QColor color ) {
      this->xStep = xStep;
      this->yStep = yStep;

      if( m_baseEntity != nullptr ) {
        m_baseEntity->deleteLater();
      }

      m_baseTransform = new Qt3DCore::QTransform();
      m_baseEntity = new Qt3DCore::QEntity( m_rootEntity );
      m_baseEntity->addComponent( m_baseTransform );
      m_baseEntity->setEnabled( gridEnabled );

      // Lines in X direction
      {
        QVector3D start( -size / 2, 0, 0 ), end( size / 2, 0, 0 );

        for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += xStep ) {
          start.setY( lineDistance );
          end.setY( lineDistance );
          drawLine( start, end,  color, m_baseEntity );
          start.setY( -lineDistance );
          end.setY( -lineDistance );
          drawLine( start, end,  color, m_baseEntity );
        }
      }

      // Lines in Y direction
      {
        QVector3D start( 0, -size / 2, 0 ), end( 0, size / 2, 0 );

        for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += yStep ) {
          start.setX( lineDistance );
          end.setX( lineDistance );
          drawLine( start, end,  color, m_baseEntity );
          start.setX( -lineDistance );
          end.setX( -lineDistance );
          drawLine( start, end,  color, m_baseEntity );
        }
      }
    }

  signals:

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;
    Qt3DCore::QEntity* m_baseEntity = nullptr;

    Qt3DCore::QTransform* m_baseTransform = nullptr;

    float xStep = 10;
    float yStep = 10;
    bool gridEnabled = true;
};

class GridModelFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    GridModelFactory( Qt3DCore::QEntity* rootEntity )
      : GuidanceFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Grid Model" );
    }

    virtual void addToCombobox( QComboBox* /*combobox*/ ) override {
    }

    virtual GuidanceBase* createNewObject() override {
      return new GridModel( rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;
};

#endif // GRIDMODEL_H

