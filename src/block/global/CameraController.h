// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QObject>

#include "3d/qt3dForwards.h"

#include <QVector3D>

#include <QtMath>

#include "block/BlockBase.h"

#include "helpers/RateLimiter.h"
#include "helpers/eigenHelper.h"

#pragma once

class CameraController : public BlockBase {
  Q_OBJECT

public:
  explicit CameraController(
    Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity, const BlockBaseId idHint, const bool systemBlock, const QString type );

  void toJSON( QJsonObject& json ) const override;
  void fromJSON( const QJsonObject& ) override;

protected:
  // CameraController also acts an EventFilter to receive the wheel-events of the mouse
  bool eventFilter( QObject*, QEvent* event ) override;

private:
  static constexpr float ZoomFactor      = 1.1f;
  static constexpr float MinZoomDistance = 10;
  static constexpr float MaxZoomDistance = 5000;
  static constexpr float TiltAngleStep   = 10;
  static constexpr float PanAngleStep    = 10;

public Q_SLOTS:
  void setMode( const int camMode );

  void setPose( POSE_SIGNATURE_SLOT );

  void tiltUp();
  void tiltDown();

  void zoomIn();
  void zoomOut();

  void panLeft();
  void panRight();

  void resetCamera();

  void setCameraSmoothing( const int orientationSmoothing, const int positionSmoothing );

private:
  void calculateOffset();

private:
  Qt3DCore::QEntity*   m_rootEntity   = nullptr;
  Qt3DRender::QCamera* m_cameraEntity = nullptr;

  Qt3DExtras::QOrbitCameraController* m_orbitController = nullptr;

  Qt3DCore::QEntity*       m_lightEntity    = nullptr;
  Qt3DRender::QPointLight* m_light          = nullptr;
  Qt3DCore::QTransform*    m_lightTransform = nullptr;

  QVector3D       m_offset            = QVector3D();
  QQuaternion     m_orientationBuffer = QQuaternion();
  Eigen::Vector3d positionBuffer      = Eigen::Vector3d( 0, 0, 0 );

  int m_mode = 0;

  double orientationSmoothing = 0.1;
  double positionSmoothing    = 0.9;
  double positionSmoothingInv = 1 - 0.9;

  float lenghtToViewCenter = 20;
  float panAngle           = 0;
  float tiltAngle          = 39;

  RateLimiter rateLimiter;
};

class CameraControllerFactory : public BlockFactory {
  Q_OBJECT

public:
  CameraControllerFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : BlockFactory( thread, true ), m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {
    typeColor = TypeColor::Model;
  }

  QString getNameOfFactory() const override { return QStringLiteral( "Camera Controller" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Graphical" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;

private:
  Qt3DCore::QEntity*   m_rootEntity   = nullptr;
  Qt3DRender::QCamera* m_cameraEntity = nullptr;
};
