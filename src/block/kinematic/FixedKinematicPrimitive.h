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

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class FixedKinematicPrimitive : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematicPrimitive()
      : BlockBase() {}

  public Q_SLOTS:
    void setOffset( const Eigen::Vector3d& offset );

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation, const PoseOption::Options& options );

  Q_SIGNALS:
    void poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

  public:
    Eigen::Vector3d positionCalculated = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();
    Eigen::Vector3d offset = Eigen::Vector3d( -1, 0, 0 );
};

class FixedKinematicPrimitiveFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicPrimitiveFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic Primitive" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
