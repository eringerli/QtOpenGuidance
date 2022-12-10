// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
      : BlockFactory( thread, false ), rootEntity( rootEntity ), usePBR( usePBR ), newOpenSaveToolbar( newOpenSaveToolbar ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Cultivated Area Model" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Graphical" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;

private:
  Qt3DCore::QEntity*  rootEntity = nullptr;
  bool                usePBR     = true;
  NewOpenSaveToolbar* newOpenSaveToolbar;
};
