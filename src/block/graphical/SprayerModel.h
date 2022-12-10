// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QObject>
#include <QPointer>

#include "3d/qt3dForwards.h"

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

class Implement;

class SprayerModel : public BlockBase {
  Q_OBJECT

public:
  explicit SprayerModel( Qt3DCore::QEntity* rootEntity, const bool usePBR );
  ~SprayerModel();

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );
  void setImplement( const QPointer< Implement >& );
  void setSections();
  void setHeight( NUMBER_SIGNATURE_SLOT );

private:
  void updateProprotions();

private:
  Qt3DCore::QEntity* m_rootEntity = nullptr;

  Qt3DCore::QTransform* m_rootEntityTransform = nullptr;

  QPointer< Implement > implement;

  std::vector< Qt3DCore::QEntity* >         boomEntities;
  std::vector< Qt3DExtras::QCylinderMesh* > boomMeshes;
  std::vector< Qt3DCore::QTransform* >      boomTransforms;
  std::vector< Qt3DCore::QEntity* >         forcedOffBoomEntities;
  std::vector< Qt3DCore::QEntity* >         forcedOnBoomEntities;
  std::vector< Qt3DCore::QEntity* >         onBoomEntities;
  std::vector< Qt3DCore::QEntity* >         offBoomEntities;
  std::vector< Qt3DCore::QEntity* >         sprayEntities;
  std::vector< Qt3DCore::QTransform* >      sprayTransforms;

  float        m_height   = 1.0;
  const QColor sprayColor = QColor( qRgb( 0x23, 0xff, 0xed ) /*Qt::lightGray*/ );
  bool         usePBR     = true;
};

class SprayerModelFactory : public BlockFactory {
  Q_OBJECT

public:
  SprayerModelFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, bool usePBR )
      : BlockFactory( thread, false ), rootEntity( rootEntity ), usePBR( usePBR ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Sprayer Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
  bool               usePBR     = true;
};
