// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class TractorModel : public BlockBase {
  Q_OBJECT

public:
  explicit TractorModel( Qt3DCore::QEntity* rootEntity, const bool usePBR );
  ~TractorModel();

public Q_SLOTS:
  void setPoseHookPoint( POSE_SIGNATURE_SLOT );
  void setPoseTowPoint( POSE_SIGNATURE_SLOT );
  void setPosePivotPoint( POSE_SIGNATURE_SLOT );

  void setSteeringAngleLeft( NUMBER_SIGNATURE_SLOT );
  void setSteeringAngleRight( NUMBER_SIGNATURE_SLOT );

  void setWheelbase( NUMBER_SIGNATURE_SLOT );
  void setTrackwidth( NUMBER_SIGNATURE_SLOT );

private:
  void setProportions();

  Qt3DCore::QEntity* m_rootEntity = nullptr;

  Qt3DCore::QEntity* m_baseEntity            = nullptr;
  Qt3DCore::QEntity* m_wheelFrontLeftEntity  = nullptr;
  Qt3DCore::QEntity* m_wheelFrontRightEntity = nullptr;
  Qt3DCore::QEntity* m_wheelBackLeftEntity   = nullptr;
  Qt3DCore::QEntity* m_wheelBackRightEntity  = nullptr;

  Qt3DCore::QEntity* m_towHookEntity    = nullptr;
  Qt3DCore::QEntity* m_pivotPointEntity = nullptr;
  Qt3DCore::QEntity* m_towPointEntity   = nullptr;

  Qt3DExtras::QCuboidMesh*   m_baseMesh       = nullptr;
  Qt3DExtras::QCylinderMesh* m_wheelFrontMesh = nullptr;
  Qt3DExtras::QCylinderMesh* m_wheelBackMesh  = nullptr;

  Qt3DExtras::QSphereMesh* m_towHookMesh    = nullptr;
  Qt3DExtras::QSphereMesh* m_pivotPointMesh = nullptr;
  Qt3DExtras::QSphereMesh* m_towPointMesh   = nullptr;

  Qt3DCore::QTransform* m_rootEntityTransform      = nullptr;
  Qt3DCore::QTransform* m_baseTransform            = nullptr;
  Qt3DCore::QTransform* m_wheelFrontLeftTransform  = nullptr;
  Qt3DCore::QTransform* m_wheelFrontRightTransform = nullptr;
  Qt3DCore::QTransform* m_wheelBackLeftTransform   = nullptr;
  Qt3DCore::QTransform* m_wheelBackRightTransform  = nullptr;

  Qt3DCore::QTransform* m_towHookTransform    = nullptr;
  Qt3DCore::QTransform* m_pivotPointTransform = nullptr;
  Qt3DCore::QTransform* m_towPointTransform   = nullptr;

  float m_wheelbase  = 2.4f;
  float m_trackwidth = 2;
};

class TractorModelFactory : public BlockFactory {
  Q_OBJECT

public:
  TractorModelFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, const bool usePBR )
      : BlockFactory( thread, false ), rootEntity( rootEntity ), usePBR( usePBR ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Tractor Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
  bool               usePBR     = true;
};
