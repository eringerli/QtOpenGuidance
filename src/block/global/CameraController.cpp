// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CameraController.h"

#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QFirstPersonCameraController>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>

#include <QEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QWheelEvent>

CameraController::CameraController( Qt3DCore::QEntity*         rootEntity,
                                    Qt3DRender::QCamera*       cameraEntity,
                                    const BlockBaseId          idHint,
                                    const bool                 systemBlock,
                                    const QString              type,
                                    const BlockBase::TypeColor typeColor )
    : BlockBase( idHint, systemBlock, type, typeColor ), m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {
  m_cameraEntity->setPosition( m_offset );
  m_cameraEntity->setViewCenter( QVector3D( 0, 0, 0 ) );
  m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );

  calculateOffset();

  m_lightEntity = new Qt3DCore::QEntity( rootEntity );
  m_light       = new Qt3DRender::QPointLight( m_lightEntity );
  m_light->setColor( "white" );
  m_light->setIntensity( 1.0f );
  m_lightEntity->addComponent( m_light );
  m_lightTransform = new Qt3DCore::QTransform( m_lightEntity );
  m_lightTransform->setTranslation( cameraEntity->position() );
  m_lightEntity->addComponent( m_lightTransform );

  // make the light follow the camera
  QObject::connect( m_cameraEntity, SIGNAL( positionChanged( QVector3D ) ), m_lightTransform, SLOT( setTranslation( QVector3D ) ) );
}

void
CameraController::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "lenghtToViewCenter" )]   = lenghtToViewCenter;
  valuesObject[QStringLiteral( "panAngle" )]             = panAngle;
  valuesObject[QStringLiteral( "tiltAngle" )]            = tiltAngle;
  valuesObject[QStringLiteral( "orientationSmoothing" )] = orientationSmoothing;
  valuesObject[QStringLiteral( "positionSmoothing" )]    = positionSmoothing;
}

void
CameraController::fromJSON( const QJsonObject& valuesObject ) {
  lenghtToViewCenter   = valuesObject[QStringLiteral( "lenghtToViewCenter" )].toDouble( 20 );
  panAngle             = valuesObject[QStringLiteral( "panAngle" )].toDouble( 0 );
  tiltAngle            = valuesObject[QStringLiteral( "tiltAngle" )].toDouble( 39 );
  orientationSmoothing = valuesObject[QStringLiteral( "orientationSmoothing" )].toDouble( 0.1 );
  positionSmoothing    = valuesObject[QStringLiteral( "positionSmoothing" )].toDouble( 0.9 );

  positionSmoothingInv = 1 - positionSmoothing;
  calculateOffset();
}

bool
CameraController::eventFilter( QObject*, QEvent* event ) {
  if( event->type() == QEvent::Wheel ) {
    auto* wheelEvent = dynamic_cast< QWheelEvent* >( event );

    if( wheelEvent->angleDelta().y() < 0 ) {
      zoomOut();
    } else {
      zoomIn();
    }
  }

  return false;
}

void
CameraController::setMode( const int camMode ) {
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

void
CameraController::setPose( const Eigen::Vector3d&           position,
                           const Eigen::Quaterniond&        orientation,
                           const CalculationOption::Options options ) {
  if( m_mode == 0 && !options.testFlag( CalculationOption::NoGraphics ) && rateLimiter.expired( RateLimiter::Type::Graphical ) ) {
    m_orientationBuffer = QQuaternion::nlerp( m_orientationBuffer, toQQuaternion( orientation ), orientationSmoothing );
    positionBuffer      = position * positionSmoothing + positionBuffer * positionSmoothingInv;

    m_cameraEntity->setPosition( toQVector3D( positionBuffer ) + ( QQuaternion::fromEulerAngles( m_orientationBuffer.toEulerAngles().x(),
                                                                                                 m_orientationBuffer.toEulerAngles().y(),
                                                                                                 m_orientationBuffer.toEulerAngles().z() ) *
                                                                   m_offset ) );
    m_cameraEntity->setViewCenter( toQVector3D( positionBuffer ) );
    m_cameraEntity->setUpVector( QVector3D( 0, 0, 1 ) );
    m_cameraEntity->rollAboutViewCenter( 0 );
    m_cameraEntity->tiltAboutViewCenter( 0 );
  }
}

void
CameraController::tiltUp() {
  tiltAngle += TiltAngleStep;

  if( tiltAngle >= 90 ) {
    tiltAngle -= TiltAngleStep;
  }

  calculateOffset();
}

void
CameraController::tiltDown() {
  tiltAngle -= TiltAngleStep;

  if( tiltAngle <= 0 ) {
    tiltAngle += TiltAngleStep;
  }

  calculateOffset();
}

void
CameraController::zoomIn() {
  lenghtToViewCenter /= ZoomFactor;

  if( lenghtToViewCenter < MinZoomDistance ) {
    lenghtToViewCenter = MinZoomDistance;
  }

  calculateOffset();
}

void
CameraController::zoomOut() {
  lenghtToViewCenter *= ZoomFactor;

  if( lenghtToViewCenter > MaxZoomDistance ) {
    lenghtToViewCenter = MaxZoomDistance;
  }

  calculateOffset();
}

void
CameraController::panLeft() {
  panAngle -= PanAngleStep;

  if( panAngle <= 0 ) {
    panAngle += 360;
  }

  calculateOffset();
}

void
CameraController::panRight() {
  panAngle += PanAngleStep;

  if( panAngle >= 360 ) {
    panAngle -= 360;
  }

  calculateOffset();
}

void
CameraController::resetCamera() {
  lenghtToViewCenter = 20;
  panAngle           = 0;
  tiltAngle          = 39;
  calculateOffset();
}

void
CameraController::setCameraSmoothing( const int orientationSmoothing, const int positionSmoothing ) {
  this->orientationSmoothing = 1 - float( orientationSmoothing ) / 100;

  this->positionSmoothingInv = double( positionSmoothing ) / 100;
  this->positionSmoothing    = 1 - positionSmoothingInv;
}

void
CameraController::calculateOffset() {
  m_offset = QQuaternion::fromAxisAndAngle( QVector3D( 0, 0, 1 ), panAngle ) *
             QQuaternion::fromAxisAndAngle( QVector3D( 0, 1, 0 ), tiltAngle ) * QVector3D( -lenghtToViewCenter, 0, 0 );
}

std::unique_ptr< BlockBase >
CameraControllerFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< CameraController >( idHint, m_rootEntity, m_cameraEntity );

  obj->addInputPort( QStringLiteral( "View Center Position" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );

  return obj;
}
