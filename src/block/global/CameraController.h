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

#include "3d/qt3dForwards.h"

#include <QVector3D>

#include <QtMath>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#pragma once

class CameraController : public BlockBase {
  Q_OBJECT

public:
  explicit CameraController( Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity );

  ~CameraController();

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

  void saveValuesToConfig();
  void loadValuesFromConfig();

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
};

class CameraControllerFactory : public BlockFactory {
  Q_OBJECT

public:
  CameraControllerFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, Qt3DRender::QCamera* cameraEntity )
      : BlockFactory( thread ), m_rootEntity( rootEntity ), m_cameraEntity( cameraEntity ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Camera Controller" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity*   m_rootEntity   = nullptr;
  Qt3DRender::QCamera* m_cameraEntity = nullptr;
};
