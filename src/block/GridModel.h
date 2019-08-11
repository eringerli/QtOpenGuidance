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

#include "BlockBase.h"

#include "../kinematic/Tile.h"

#include "../3d/linemesh.h"

class GridModel : public BlockBase {
    Q_OBJECT

  public:
    explicit GridModel( Qt3DCore::QEntity* rootEntity ) {
      m_baseEntity = new Qt3DCore::QEntity( rootEntity );

      m_baseTransform = new Qt3DCore::QTransform();
      m_baseEntity->addComponent( m_baseTransform );

      m_lineMesh = new LineMesh();
      m_baseEntity->addComponent( m_lineMesh );

      m_material = new Qt3DExtras::QPhongMaterial( m_baseEntity );
      m_baseEntity->addComponent( m_material );
    }

    ~GridModel() {
      m_lineMesh->deleteLater();
      m_material->deleteLater();
      m_baseTransform->deleteLater();
      m_baseEntity->deleteLater();
    }

  public slots:
    void setPose( Tile* tile, QVector3D position, QQuaternion ) {
      m_baseEntity->setParent( tile->tileEntity );
      QVector3D positionModulo( ( std::floor( ( tile->x + position.x() ) / xStep )*xStep ) - tile->x,
                                ( std::floor( ( tile->y + position.y() ) / yStep )*yStep ) - tile->y,
                                position.z() );
      m_baseTransform->setTranslation( positionModulo );
    }

    void setGrid( bool enabled ) {
      m_baseEntity->setEnabled( enabled );
    }

    void setGridValues( float xStep, float yStep, float size, QColor color ) {
      this->xStep = xStep;
      this->yStep = yStep;

      QVector<QVector3D> lines;

      // Lines in X direction
      {
        QVector3D start( -size / 2, 0, 0 ), end( size / 2, 0, 0 );

        for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += yStep ) {
          start.setY( lineDistance );
          end.setY( lineDistance );
          lines.append( start );
          lines.append( end );

          start.setY( -lineDistance );
          end.setY( -lineDistance );
          lines.append( start );
          lines.append( end );
        }
      }

      // Lines in Y direction
      {
        QVector3D start( 0, -size / 2, 0 ), end( 0, size / 2, 0 );

        for( float lineDistance = 0; lineDistance < ( size / 2 ); lineDistance += xStep ) {
          start.setX( lineDistance );
          end.setX( lineDistance );
          lines.append( start );
          lines.append( end );

          start.setX( -lineDistance );
          end.setX( -lineDistance );
          lines.append( start );
          lines.append( end );
        }
      }

      m_material->setAmbient( color );

      m_lineMesh->posUpdate( lines );
    }

  signals:

  private:
    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;
    LineMesh* m_lineMesh = nullptr;
    Qt3DExtras::QPhongMaterial* m_material = nullptr;

    float xStep = 10;
    float yStep = 10;
};

class GridModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    GridModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Grid Model" );
    }

    virtual void addToCombobox( QComboBox* /*combobox*/ ) override {
    }

    virtual BlockBase* createNewObject() override {
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
};

#endif // GRIDMODEL_H

