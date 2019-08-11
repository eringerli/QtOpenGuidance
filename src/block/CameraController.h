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

#include <QObject>

#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

#include "BlockBase.h"

#include "../kinematic/Tile.h"

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

class CameraController : public BlockBase {
    Q_OBJECT

  public:
    explicit CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : m_mode( 0 ), m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ), m_orbitController( nullptr ) {
      m_cameraEntity->setPosition( m_offset );
      m_cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
      m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );

      calculateOffset();

      m_lightEntity = new Qt3DCore::QEntity( rootEntity );
      m_light = new Qt3DRender::QPointLight( m_lightEntity );
      m_light->setColor( "white" );
      m_light->setIntensity( 1.0f );
      m_lightEntity->addComponent( m_light );
      m_lightTransform = new Qt3DCore::QTransform( m_lightEntity );
      m_lightTransform->setTranslation( cameraEntity->position() );
      m_lightEntity->addComponent( m_lightTransform );

      // make the light follow the camera
      QObject::connect( m_cameraEntity, SIGNAL( positionChanged( QVector3D ) ),
                        m_lightTransform, SLOT( setTranslation( QVector3D ) ) );

      loadValuesFromConfig();
      calculateOffset();
    }

  public slots:
    void setMode( int camMode ) {
      m_mode = camMode;

      if( camMode == 0 ) {
        if( m_orbitController != nullptr ) {
          delete m_orbitController;
          m_orbitController = nullptr;
        }
      } else {
        if( m_orbitController == nullptr ) {
          m_orbitController = new Qt3DExtras::QOrbitCameraController( m_rootEntity );
          m_orbitController->setCamera( m_cameraEntity );
          m_orbitController->setLinearSpeed( 150 );
        }
      }
    }

    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {
      if( m_mode == 0 ) {
        m_cameraEntity->setParent( tile->tileEntity );
        m_lightEntity->setParent( tile->tileEntity );

        m_cameraEntity->setPosition( position + ( orientation * m_offset ) );
        m_cameraEntity->setViewCenter( position );
        m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
      }
    }

    void tiltUp() {
      tiltAngle += 10;

      if( tiltAngle >= 90 ) {
        tiltAngle -= 10;
      }

      calculateOffset();
    }
    void tiltDown() {
      tiltAngle -= 10;

      if( tiltAngle <= 0 ) {
        tiltAngle += 10;
      }

      calculateOffset();
    }

    void zoomIn() {
      lenghtToViewCenter /= 1.2F;

      if( lenghtToViewCenter < 10 ) {
        lenghtToViewCenter *= 1.2F;
      }

      calculateOffset();
    }
    void zoomOut() {
      lenghtToViewCenter *= 1.2F;

      if( lenghtToViewCenter > 200 ) {
        lenghtToViewCenter /= 1.2F;
      }

      calculateOffset();
    }

    void panLeft() {
      panAngle -= 10;

      if( panAngle <= 0 ) {
        panAngle += 360;
      }

      calculateOffset();
    }
    void panRight() {
      panAngle += 10;

      if( panAngle >= 360 ) {
        panAngle -= 360;
      }

      calculateOffset();
    }

    void resetCamera() {
      lenghtToViewCenter = 20;
      panAngle = 0;
      tiltAngle = 39;
      calculateOffset();
    }

    void calculateOffset() {
      m_offset =
        QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), panAngle ) *
        QQuaternion::fromAxisAndAngle( QVector3D( 0, 1, 0 ), tiltAngle ) *
        QVector3D( -lenghtToViewCenter, 0, 0 );
    }

    void saveValuesToConfig() {
      QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                          QSettings::IniFormat );

      settings.setValue( "Camera/lenghtToViewCenter", lenghtToViewCenter );
      settings.setValue( "Camera/panAngle", panAngle );
      settings.setValue( "Camera/tiltAngle", tiltAngle );
    }

    void loadValuesFromConfig() {
      QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                          QSettings::IniFormat );

      lenghtToViewCenter = settings.value( "Camera/lenghtToViewCenter", 20 ).toFloat();
      panAngle = settings.value( "Camera/panAngle", 0 ).toFloat();
      tiltAngle = settings.value( "Camera/tiltAngle", 39 ).toFloat();
    }

  private:
    int m_mode;

    Qt3DCore::QEntity* m_rootEntity;
    Qt3DRender::QCamera* m_cameraEntity;

    Qt3DExtras::QOrbitCameraController* m_orbitController;

    QVector3D m_offset;
    float lenghtToViewCenter = 20;
    float panAngle = 0;
    float tiltAngle = 39;

    Qt3DCore::QEntity* m_lightEntity;
    Qt3DRender::QPointLight* m_light;
    Qt3DCore::QTransform* m_lightTransform;
};

class CameraControllerFactory : public BlockFactory {
    Q_OBJECT

  public:
    CameraControllerFactory( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : BlockFactory(),
        m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Camera Controller" );
    }

    virtual void addToCombobox( QComboBox* ) override {
    }

    virtual BlockBase* createNewObject() override {
      return new CameraController( m_rootEntity, m_cameraEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );
      b->addInputPort( "View Center Position", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* m_rootEntity;
    Qt3DRender::QCamera* m_cameraEntity;
};

#endif // CAMERACONTROLLER_H
