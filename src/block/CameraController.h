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

#include "GuidanceBase.h"

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

class CameraController : public GuidanceBase {

    Q_OBJECT

  public:
    explicit CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : m_mode( 0 ), m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ), m_orbitController( nullptr ),
        m_offset( -20, 0, 10 ) {
      m_cameraEntity->setPosition( m_offset );
      m_cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
      m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
    }

    ~CameraController() {}

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

    void setPose( QVector3D position, QQuaternion orientation ) {
      if( m_mode == 0 ) {
        m_cameraEntity->setPosition( position + ( orientation * m_offset ) );
        m_cameraEntity->setViewCenter( position );
        m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
      }
    }

    void tiltUp() {
      m_offset = QQuaternion::fromAxisAndAngle( QVector3D( 0, 1, 0 ), 10 ) *  m_offset;
    }
    void tiltDown() {
      m_offset = QQuaternion::fromAxisAndAngle( QVector3D( 0, 1, 0 ), -10 ) *  m_offset;
    }

    void zoomIn() {
      m_offset /= 1.2F;
    }
    void zoomOut() {

      m_offset *= 1.2F;
    }

    void panLeft() {
      m_offset = QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), -10 ) *  m_offset;
    }
    void panRight() {
      m_offset = QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), 10 ) *  m_offset;
    }

    void resetCamera() {
      m_offset = QVector3D( -20, 0, 10 );
    }

  private:
    int m_mode;

    Qt3DCore::QEntity* m_rootEntity;
    Qt3DRender::QCamera* m_cameraEntity;

    Qt3DExtras::QOrbitCameraController* m_orbitController;

    QVector3D m_offset;

};

class CameraControllerFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    CameraControllerFactory( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : GuidanceFactory(),
        m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Camera Controller" );
    }

    virtual void addToCombobox( QComboBox* ) override {
    }

    virtual GuidanceBase* createNewObject() override {
      return new CameraController( m_rootEntity, m_cameraEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( QStringLiteral( "Camera" ), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );
      b->addInputPort( "View Center Position", SLOT( setPose( QVector3D, QQuaternion ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* m_rootEntity;
    Qt3DRender::QCamera* m_cameraEntity;
};

#endif // CAMERACONTROLLER_H
