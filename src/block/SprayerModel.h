// Copyright( C ) 2019 Christian Riggenbach
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

#ifndef SPRAYERMODEL_H
#define SPRAYERMODEL_H

#include <QObject>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
//#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QMetalRoughMaterial>

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
    void setPose( Point_3, QQuaternion, PoseOption::Options );
    void setImplement( QPointer<Implement> );
    void setSections( QPointer<Implement> );
    void setHeight( float );

  private:
    void updateProprotions();

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;

    Qt3DCore::QTransform* m_rootEntityTransform = nullptr;

    QPointer<Implement> implement;

    float m_height = 1.0;
    QColor sprayerColor = QColor( qRgb/*a*/( 0x23, 0xff, 0xed/*, 255*/ ) );

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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new SprayerModel( rootEntity );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort(QStringLiteral("Pose"),QStringLiteral(SLOT( setPose( Point_3, QQuaternion, PoseOption::Options ) )));
      b->addInputPort(QStringLiteral("Height"),QStringLiteral(SLOT( setHeight( float ) )));
      b->addInputPort(QStringLiteral("Implement Data"),QStringLiteral(SLOT( setImplement( QPointer<Implement> ) )));
      b->addInputPort(QStringLiteral("Section Control Data"),QStringLiteral(SLOT( setSections( QPointer<Implement> ) )));

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};

#endif // SPRAYERMODEL_H
