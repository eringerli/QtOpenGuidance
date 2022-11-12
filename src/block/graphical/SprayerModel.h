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
      : BlockFactory( thread ), rootEntity( rootEntity ), usePBR( usePBR ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Sprayer Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity* rootEntity = nullptr;
  bool               usePBR     = true;
};
