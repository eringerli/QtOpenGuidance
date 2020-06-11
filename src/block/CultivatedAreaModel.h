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

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QLayer>

#include <Qt3DExtras/QMetalRoughMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>

#include "BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/PoseOptions.h"

#include "../block/Implement.h"

class CgalThread;
class CultivatedAreaMesh;

class CultivatedAreaModel : public BlockBase {
    Q_OBJECT

  public:
    explicit CultivatedAreaModel( Qt3DCore::QEntity* rootEntity, CgalThread* threadForCgalWorker );
    ~CultivatedAreaModel();

    virtual void emitConfigSignals() override;

  public slots:
    void setPose( const Point_3, const QQuaternion, const PoseOption::Options );
    void setImplement( const QPointer<Implement>& );
    void setSections();

  signals:
    void layerChanged( Qt3DRender::QLayer* );

  private:
    CultivatedAreaMesh* createNewMesh();

  private:
    CgalThread* threadForCgalWorker = nullptr;

    Qt3DCore::QEntity* m_baseEntity = nullptr;
    Qt3DCore::QTransform* m_baseTransform = nullptr;

    Qt3DExtras::QMetalRoughMaterial* m_pbrMaterial = nullptr;
    Qt3DExtras::QDiffuseSpecularMaterial* m_phongMaterial = nullptr;

    Qt3DRender::QLayer* m_layer;

    QPointer<Implement> implement;

    std::vector<double> sectionOffsets;
    std::vector<CultivatedAreaMesh*> sectionMeshes;
};

class CultivatedAreaModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    CultivatedAreaModelFactory( Qt3DCore::QEntity* rootEntity, bool usePBR );

    QString getNameOfFactory() override {
      return QStringLiteral( "Cultivated Area Model" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new CultivatedAreaModel( rootEntity, threadForCgalWorker );
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Implement Data" ), QLatin1String( SLOT( setImplement( const QPointer<Implement> ) ) ) );
      b->addInputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SLOT( setSections() ) ) );

      b->addOutputPort( QStringLiteral( "Cultivated Area" ), QLatin1String( SIGNAL( layerChanged( Qt3DRender::QLayer* ) ) ) );

      b->setBrush( modelColor );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    bool usePBR = true;
    CgalThread* threadForCgalWorker = nullptr;
};
