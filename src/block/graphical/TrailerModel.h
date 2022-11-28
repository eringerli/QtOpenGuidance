// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class TrailerModel : public BlockBase {
  Q_OBJECT

public:
  explicit TrailerModel( Qt3DCore::QEntity* rootEntity, const bool usePBR );
  ~TrailerModel();

public Q_SLOTS:
  void setPoseHookPoint( POSE_SIGNATURE_SLOT );
  void setPoseTowPoint( POSE_SIGNATURE_SLOT );
  void setPosePivotPoint( POSE_SIGNATURE_SLOT );

  void setOffsetHookPointPosition( const Eigen::Vector3d& position );
  void setTrackwidth( NUMBER_SIGNATURE_SLOT );

private:
  void setProportions();

private:
  Qt3DCore::QEntity* m_rootEntity = nullptr;

  Qt3DCore::QEntity* m_hitchEntity      = nullptr;
  Qt3DCore::QEntity* m_axleEntity       = nullptr;
  Qt3DCore::QEntity* m_wheelLeftEntity  = nullptr;
  Qt3DCore::QEntity* m_wheelRightEntity = nullptr;

  Qt3DExtras::QCylinderMesh* m_hitchMesh = nullptr;
  Qt3DExtras::QCylinderMesh* m_axleMesh  = nullptr;
  Qt3DExtras::QCylinderMesh* m_wheelMesh = nullptr;

  Qt3DCore::QTransform* m_rootEntityTransform = nullptr;
  Qt3DCore::QTransform* m_hitchTransform      = nullptr;
  Qt3DCore::QTransform* m_axleTransform       = nullptr;
  Qt3DCore::QTransform* m_wheelLeftTransform  = nullptr;
  Qt3DCore::QTransform* m_wheelRightTransform = nullptr;

  Qt3DCore::QEntity* m_towHookEntity    = nullptr;
  Qt3DCore::QEntity* m_pivotPointEntity = nullptr;
  Qt3DCore::QEntity* m_towPointEntity   = nullptr;

  Qt3DExtras::QSphereMesh* m_towHookMesh    = nullptr;
  Qt3DExtras::QSphereMesh* m_pivotPointMesh = nullptr;
  Qt3DExtras::QSphereMesh* m_towPointMesh   = nullptr;

  Qt3DCore::QTransform* m_towHookTransform    = nullptr;
  Qt3DCore::QTransform* m_pivotPointTransform = nullptr;
  Qt3DCore::QTransform* m_towPointTransform   = nullptr;

  Eigen::Vector3d m_offsetHookPoint = Eigen::Vector3d( 6, 0, 0 );
  double          m_trackwidth      = 2.4f;
};

class TrailerModelFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerModelFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, const bool usePBR )
      : BlockFactory( thread ), rootEntity( rootEntity ), usePBR( usePBR ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Trailer Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
  bool               usePBR     = true;
};
