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
#include "kinematic/PoseOptions.h"

class TrailerModel : public BlockBase {
    Q_OBJECT

  public:
    explicit TrailerModel( Qt3DCore::QEntity* rootEntity, const bool usePBR );
    ~TrailerModel();

  public Q_SLOTS:
    void setPoseHookPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void setPoseTowPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void setPosePivotPoint( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

    void setOffsetHookPointPosition( const Eigen::Vector3d& position );
    void setTrackwidth( const double trackwidth );

  private:
    void setProportions();

  private:
    Qt3DCore::QEntity* m_rootEntity = nullptr;

    Qt3DCore::QEntity* m_hitchEntity = nullptr;
    Qt3DCore::QEntity* m_axleEntity = nullptr;
    Qt3DCore::QEntity* m_wheelLeftEntity = nullptr;
    Qt3DCore::QEntity* m_wheelRightEntity = nullptr;

    Qt3DExtras::QCylinderMesh* m_hitchMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_axleMesh = nullptr;
    Qt3DExtras::QCylinderMesh* m_wheelMesh = nullptr;

    Qt3DCore::QTransform* m_rootEntityTransform = nullptr;
    Qt3DCore::QTransform* m_hitchTransform = nullptr;
    Qt3DCore::QTransform* m_axleTransform = nullptr;
    Qt3DCore::QTransform* m_wheelLeftTransform = nullptr;
    Qt3DCore::QTransform* m_wheelRightTransform = nullptr;


    Qt3DCore::QEntity* m_towHookEntity = nullptr;
    Qt3DCore::QEntity* m_pivotPointEntity = nullptr;
    Qt3DCore::QEntity* m_towPointEntity = nullptr;

    Qt3DExtras::QSphereMesh* m_towHookMesh = nullptr;
    Qt3DExtras::QSphereMesh* m_pivotPointMesh = nullptr;
    Qt3DExtras::QSphereMesh* m_towPointMesh = nullptr;

    Qt3DCore::QTransform* m_towHookTransform = nullptr;
    Qt3DCore::QTransform* m_pivotPointTransform = nullptr;
    Qt3DCore::QTransform* m_towPointTransform = nullptr;

    Eigen::Vector3d m_offsetHookPoint = Eigen::Vector3d( 6, 0, 0 );
    double m_trackwidth = 2.4f;
};

class TrailerModelFactory : public BlockFactory {
    Q_OBJECT

  public:
    TrailerModelFactory( Qt3DCore::QEntity* rootEntity, const bool usePBR )
      : BlockFactory(),
        rootEntity( rootEntity ),
        usePBR( usePBR ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Model" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Graphical" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;

  private:
    Qt3DCore::QEntity* rootEntity = nullptr;
    bool usePBR = true;
};
