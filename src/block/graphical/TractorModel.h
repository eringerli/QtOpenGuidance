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
      : BlockFactory( thread ), rootEntity( rootEntity ), usePBR( usePBR ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Tractor Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
  bool               usePBR     = true;
};
