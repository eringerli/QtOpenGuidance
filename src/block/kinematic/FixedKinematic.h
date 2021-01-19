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
#include "FixedKinematicPrimitive.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class FixedKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : BlockBase() {}

  public Q_SLOTS:
    void setOffsetHookToPivot( const Eigen::Vector3d& offset );
    void setOffsetPivotToTow( const Eigen::Vector3d& offset );

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options );

  Q_SIGNALS:
    void poseHookPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void posePivotPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void poseTowPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

  private:

    FixedKinematicPrimitive hookToPivot;
    FixedKinematicPrimitive pivotToTow;
};

class FixedKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
