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

#include <Qt3DExtras/QCylinderMesh>

#include "BlockBase.h"

#include "../cgalKernel.h"

#include "../kinematic/PoseOptions.h"

#include "../block/Implement.h"

class SprayerModel : public BlockBase {
    Q_OBJECT

  public:
    explicit SprayerModel( Qt3DCore::QEntity* rootEntity );
    ~SprayerModel();

  public slots:
    void setPose( const Point_3&, const QQuaternion, const PoseOption::Options );
    void setImplement( const QPointer<Implement>& );
    void setSections();
    void setHeight( double );

  private:
    void updateProprotions();

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;

    Qt3DCore::QTransform* m_rootEntityTransform = nullptr;

    QPointer<Implement> implement;

    std::vector<Qt3DCore::QEntity*> boomEntities;
    std::vector<Qt3DExtras::QCylinderMesh*> boomMeshes;
    std::vector<Qt3DCore::QTransform*> boomTransforms;
    std::vector<Qt3DCore::QEntity*> forcedOffBoomEntities;
    std::vector<Qt3DCore::QEntity*> forcedOnBoomEntities;
    std::vector<Qt3DCore::QEntity*> onBoomEntities;
    std::vector<Qt3DCore::QEntity*> offBoomEntities;
    std::vector<Qt3DCore::QEntity*> sprayEntities;
    std::vector<Qt3DCore::QTransform*> sprayTransforms;

    float m_height = 1.0;
    const QColor sprayColor = QColor( qRgb( 0x23, 0xff, 0xed ) /*Qt::lightGray*/ );

};

class SprayerModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    SprayerModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Sprayer Model" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new SprayerModel( rootEntity );
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Height" ), QLatin1String( SLOT( setHeight( double ) ) ) );
      b->addInputPort( QStringLiteral( "Implement Data" ), QLatin1String( SLOT( setImplement( const QPointer<Implement> ) ) ) );
      b->addInputPort( QStringLiteral( "Section Control Data" ), QLatin1String( SLOT( setSections() ) ) );

      b->setBrush( QColor( QStringLiteral( "moccasin" ) ) );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};
