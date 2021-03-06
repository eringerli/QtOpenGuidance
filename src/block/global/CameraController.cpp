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

#include "CameraController.h"

#include "qneblock.h"
#include "qneport.h"

#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

#include <QSettings>
#include <QEvent>
#include <QWheelEvent>
#include <QStandardPaths>

CameraController::CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
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

CameraController::~CameraController() {
  saveValuesToConfig();
}

bool CameraController::eventFilter( QObject*, QEvent* event ) {

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

void CameraController::setMode( const int camMode ) {
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

void CameraController::setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
  if( m_mode == 0 && !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {

    m_orientationBuffer = QQuaternion::nlerp( m_orientationBuffer, toQQuaternion( orientation ), orientationSmoothing );
    positionBuffer = position * positionSmoothing + positionBuffer * positionSmoothingInv;

    m_cameraEntity->setPosition( toQVector3D( positionBuffer ) +
                                 ( QQuaternion::fromEulerAngles( m_orientationBuffer.toEulerAngles().x(), m_orientationBuffer.toEulerAngles().y(), m_orientationBuffer.toEulerAngles().z() ) * m_offset ) );
    m_cameraEntity->setViewCenter( toQVector3D( positionBuffer ) );
    m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
    m_cameraEntity->rollAboutViewCenter( 0 );
    m_cameraEntity->tiltAboutViewCenter( 0 );
  }
}

void CameraController::tiltUp() {
  tiltAngle += TiltAngleStep;

  if( tiltAngle >= 90 ) {
    tiltAngle -= TiltAngleStep;
  }

  calculateOffset();
}

void CameraController::tiltDown() {
  tiltAngle -= TiltAngleStep;

  if( tiltAngle <= 0 ) {
    tiltAngle += TiltAngleStep;
  }

  calculateOffset();
}

void CameraController::zoomIn() {
  lenghtToViewCenter /= ZoomFactor;

  if( lenghtToViewCenter < MinZoomDistance ) {
    lenghtToViewCenter = MinZoomDistance;
  }

  calculateOffset();
}

void CameraController::zoomOut() {
  lenghtToViewCenter *= ZoomFactor;

  if( lenghtToViewCenter > MaxZoomDistance ) {
    lenghtToViewCenter = MaxZoomDistance;
  }

  calculateOffset();
}

void CameraController::panLeft() {
  panAngle -= PanAngleStep;

  if( panAngle <= 0 ) {
    panAngle += 360;
  }

  calculateOffset();
}

void CameraController::panRight() {
  panAngle += PanAngleStep;

  if( panAngle >= 360 ) {
    panAngle -= 360;
  }

  calculateOffset();
}

void CameraController::resetCamera() {
  lenghtToViewCenter = 20;
  panAngle = 0;
  tiltAngle = 39;
  calculateOffset();
}

void CameraController::setCameraSmoothing( const int orientationSmoothing, const int positionSmoothing ) {
  this->orientationSmoothing = 1 - float( orientationSmoothing ) / 100;

  this->positionSmoothingInv = double( positionSmoothing ) / 100;
  this->positionSmoothing = 1 - positionSmoothingInv;

  saveValuesToConfig();
}

void CameraController::calculateOffset() {
  m_offset =
          QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), panAngle ) *
          QQuaternion::fromAxisAndAngle( QVector3D( 0, 1, 0 ), tiltAngle ) *
          QVector3D( -lenghtToViewCenter, 0, 0 );
}

void CameraController::saveValuesToConfig() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  settings.setValue( QStringLiteral( "Camera/lenghtToViewCenter" ), lenghtToViewCenter );
  settings.setValue( QStringLiteral( "Camera/panAngle" ), panAngle );
  settings.setValue( QStringLiteral( "Camera/tiltAngle" ), tiltAngle );
  settings.setValue( QStringLiteral( "Camera/orientationSmoothing" ), float( orientationSmoothing ) );
  settings.setValue( QStringLiteral( "Camera/positionSmoothing" ), positionSmoothing );

  settings.sync();
}

void CameraController::loadValuesFromConfig() {
  QSettings settings( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) + "/config.ini",
                      QSettings::IniFormat );

  lenghtToViewCenter = settings.value( QStringLiteral( "Camera/lenghtToViewCenter" ), 20 ).toFloat();
  panAngle = settings.value( QStringLiteral( "Camera/panAngle" ), 0 ).toFloat();
  tiltAngle = settings.value( QStringLiteral( "Camera/tiltAngle" ), 39 ).toFloat();
}

QNEBlock* CameraControllerFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new CameraController( m_rootEntity, m_cameraEntity );
  auto* b = createBaseBlock( scene, obj, id, true );

  b->addInputPort( QStringLiteral( "View Center Position" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

  return b;
}
