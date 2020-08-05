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

#include <QVector2D>
#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <Qt3DRender/QAttribute>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QTextureMaterial>

#include <QDebug>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../kinematic/PathPrimitive.h"
#include "../kinematic/Plan.h"

#include "../3d/ArrowTexture.h"
#include "../3d/BufferMesh.h"

#include <QVector>
#include <QSharedPointer>
#include <utility>

class PathPlannerModel : public BlockBase {
    Q_OBJECT

  public:
    explicit PathPlannerModel( Qt3DCore::QEntity* rootEntity );

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;
      valuesObject[QStringLiteral( "visible" )] = visible;
      valuesObject[QStringLiteral( "zOffset" )] = zOffset;
      valuesObject[QStringLiteral( "viewBox" )] = viewBox;
      valuesObject[QStringLiteral( "individualSegmentColor" )] = individualSegmentColor;
      valuesObject[QStringLiteral( "individualRayColor" )] = individualRayColor;
      valuesObject[QStringLiteral( "lineColor" )] = linesColor.name();
      valuesObject[QStringLiteral( "segmentColor" )] = segmentsColor.name();
      valuesObject[QStringLiteral( "rayColor" )] = raysColor.name();

      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        visible = valuesObject[QStringLiteral( "visible" )].toBool( true );
        zOffset = valuesObject[QStringLiteral( "zOffset" )].toDouble( 0.1 );
        viewBox = valuesObject[QStringLiteral( "viewBox" )].toDouble( 50 );
        individualSegmentColor = valuesObject[QStringLiteral( "individualSegmentColor" )].toBool( false );
        individualRayColor = valuesObject[QStringLiteral( "individualRayColor" )].toBool( false );
        linesColor = QColor( valuesObject[QStringLiteral( "lineColor" )].toString( "#00ff00" ) );
        segmentsColor = QColor( valuesObject[QStringLiteral( "segmentColor" )].toString( "#00ff00" ) );
        raysColor = QColor( valuesObject[QStringLiteral( "rayColor" )].toString( "#00ff00" ) );

        refreshColors();
      }
    }

    void refreshColors();

  public slots:
    void setVisible( bool visible ) {
      baseEntity->setEnabled( visible );
    }

    void setPlan( const Plan& plan ) {
      this->plan = plan;
    }

    void setPose( const Point_3 position, const Eigen::Quaterniond orientation, const PoseOption::Options options );

  public:
    Point_3 position = Point_3( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();

    bool visible = true;
    double zOffset = 0.1;
    double viewBox = 50;
    bool individualSegmentColor = false;
    bool individualRayColor = false;
    bool bisectorsVisible = false;

    QColor linesColor = QColor( Qt::green );
    QColor segmentsColor = QColor( Qt::green );
    QColor raysColor = QColor( Qt::green );
    QColor bisectorsColor = QColor( Qt::blue );

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;

    Qt3DCore::QEntity* baseEntity = nullptr;

    Qt3DCore::QEntity* linesEntity = nullptr;
    Qt3DCore::QEntity* raysEntity = nullptr;
    Qt3DCore::QEntity* segmentsEntity = nullptr;
    Qt3DCore::QEntity* bisectorsEntity = nullptr;
    BufferMesh* linesMesh = nullptr;
    BufferMesh* raysMesh = nullptr;
    BufferMesh* segmentsMesh = nullptr;
    BufferMesh* bisectorsMesh = nullptr;
    Qt3DExtras::QPhongMaterial* linesMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* raysMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* segmentsMaterial = nullptr;
    Qt3DExtras::QPhongMaterial* bisectorsMaterial = nullptr;

  private:
    Plan plan;
};

class PathPlannerModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    PathPlannerModelFactory( Qt3DCore::QEntity* rootEntity )
      : BlockFactory(),
        rootEntity( rootEntity ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Path Planner Model" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new PathPlannerModel( rootEntity );
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Plan" ), QLatin1String( SLOT( setPlan( const Plan& ) ) ) );

      b->setBrush( modelColor );

      return b;
    }

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
};
