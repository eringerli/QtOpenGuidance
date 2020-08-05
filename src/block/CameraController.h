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

#include <QObject>

#include <QVector3D>
#include <Qt3DCore/QTransform>

#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

#include <QtMath>

#include "BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#pragma once

class CameraController : public BlockBase {
    Q_OBJECT

  public:
    explicit CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {
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

  protected:
    // CameraController also acts an EventFilter to receive the wheel-events of the mouse
    bool eventFilter( QObject*, QEvent* event ) override {

      if( event->type() == QEvent::Wheel ) {
        auto* wheelEvent = dynamic_cast<QWheelEvent*>( event );

        if( wheelEvent->angleDelta().y() < 0 ) {
          zoomOut();
        } else {
          zoomIn();
        }
      }

      return false;
    }

  private:
    static constexpr float ZoomFactor = 1.1f;
    static constexpr float MinZoomDistance = 10;
    static constexpr float MaxZoomDistance = 1000;
    static constexpr float TiltAngleStep = 10;
    static constexpr float PanAngleStep = 10;

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

    void setPose( const Point_3 position, Eigen::Quaterniond orientation, PoseOption::Options options ) {
      if( m_mode == 0 && !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        m_cameraEntity->setPosition( convertPoint3ToQVector3D( position ) +
                                     ( QQuaternion::fromEulerAngles( 0, 0, toQQuaternion( orientation ).toEulerAngles().z() ) * m_offset ) );
        m_cameraEntity->setViewCenter( convertPoint3ToQVector3D( position ) );
        m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
        m_cameraEntity->rollAboutViewCenter( 0 );
        m_cameraEntity->tiltAboutViewCenter( 0 );
      }
    }

    void tiltUp() {
      tiltAngle += TiltAngleStep;

      if( tiltAngle >= 90 ) {
        tiltAngle -= TiltAngleStep;
      }

      calculateOffset();
    }
    void tiltDown() {
      tiltAngle -= TiltAngleStep;

      if( tiltAngle <= 0 ) {
        tiltAngle += TiltAngleStep;
      }

      calculateOffset();
    }

    void zoomIn() {
      lenghtToViewCenter /= ZoomFactor;

      if( lenghtToViewCenter < MinZoomDistance ) {
        lenghtToViewCenter = MinZoomDistance;
      }

      calculateOffset();
    }
    void zoomOut() {
      lenghtToViewCenter *= ZoomFactor;

      if( lenghtToViewCenter > MaxZoomDistance ) {
        lenghtToViewCenter = MaxZoomDistance;
      }

      calculateOffset();
    }

    void panLeft() {
      panAngle -= PanAngleStep;

      if( panAngle <= 0 ) {
        panAngle += 360;
      }

      calculateOffset();
    }
    void panRight() {
      panAngle += PanAngleStep;

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

      settings.setValue( QStringLiteral( "Camera/lenghtToViewCenter" ), lenghtToViewCenter );
      settings.setValue( QStringLiteral( "Camera/panAngle" ), panAngle );
      settings.setValue( QStringLiteral( "Camera/tiltAngle" ), tiltAngle );
    }

    void loadValuesFromConfig() {
      QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                          QSettings::IniFormat );

      lenghtToViewCenter = settings.value( QStringLiteral( "Camera/lenghtToViewCenter" ), 20 ).toFloat();
      panAngle = settings.value( QStringLiteral( "Camera/panAngle" ), 0 ).toFloat();
      tiltAngle = settings.value( QStringLiteral( "Camera/tiltAngle" ), 39 ).toFloat();
    }

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;
    Qt3DRender::QCamera* m_cameraEntity = nullptr;

    Qt3DExtras::QOrbitCameraController* m_orbitController = nullptr;

    Qt3DCore::QEntity* m_lightEntity = nullptr;
    Qt3DRender::QPointLight* m_light = nullptr;
    Qt3DCore::QTransform* m_lightTransform = nullptr;

    QVector3D m_offset = QVector3D();

    int m_mode = 0;

    float lenghtToViewCenter = 20;
    float panAngle = 0;
    float tiltAngle = 39;
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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new CameraController( m_rootEntity, m_cameraEntity );
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addInputPort( QStringLiteral( "View Center Position" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;
    Qt3DRender::QCamera* m_cameraEntity = nullptr;
};
