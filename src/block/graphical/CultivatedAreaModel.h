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
#include <QPointer>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DCore/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>

#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include "../sectionControl/Implement.h"

class CultivatedAreaMesh;
class Implement;

class NewOpenSaveToolbar;
class QFile;

class CultivatedAreaModel : public BlockBase {
  Q_OBJECT

public:
  explicit CultivatedAreaModel( Qt3DCore::QEntity* rootEntity, NewOpenSaveToolbar* newOpenSaveToolbar );
  ~CultivatedAreaModel();

  virtual void emitConfigSignals() override;

public Q_SLOTS:
  void setPose( POSE_SIGNATURE_SLOT );
  void setImplement( const QPointer< Implement >& );
  void setSections();

  void newCultivatedArea();

  void openCultivatedArea();
  void openCultivatedAreaFromFile( QFile& file );

  void saveCultivatedArea();
  void saveCultivatedAreaToFile( QFile& file );

Q_SIGNALS:
  void layerChanged( Qt3DRender::QLayer* );

private:
  CultivatedAreaMesh* createNewMesh();
  void                createEntities();

private:
  NewOpenSaveToolbar* newOpenSaveToolbar;

  QAction* newCultivatedAreaAction;
  QAction* openCultivatedAreaAction;
  QAction* saveCultivatedAreaAction;

  Qt3DCore::QEntity* m_rootEntity = nullptr;

  Qt3DCore::QEntity*    m_baseEntity    = nullptr;
  Qt3DCore::QTransform* m_baseTransform = nullptr;

  Qt3DExtras::QMetalRoughMaterial*      m_pbrMaterial   = nullptr;
  Qt3DExtras::QDiffuseSpecularMaterial* m_phongMaterial = nullptr;

  Qt3DRender::QLayer* m_layer;

  QPointer< Implement > implement;

  std::vector< double >              sectionOffsets;
  std::vector< bool >                sectionStates;
  std::vector< CultivatedAreaMesh* > sectionMeshes;
};

class CultivatedAreaModelFactory : public BlockFactory {
  Q_OBJECT

public:
  CultivatedAreaModelFactory( QThread* thread, Qt3DCore::QEntity* rootEntity, bool usePBR, NewOpenSaveToolbar* newOpenSaveToolbar )
      : BlockFactory( thread ), rootEntity( rootEntity ), usePBR( usePBR ), newOpenSaveToolbar( newOpenSaveToolbar ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Cultivated Area Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

private:
  Qt3DCore::QEntity*  rootEntity = nullptr;
  bool                usePBR     = true;
  NewOpenSaveToolbar* newOpenSaveToolbar;
};
