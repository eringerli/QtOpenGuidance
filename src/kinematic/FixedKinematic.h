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

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "../block/BlockBase.h"
#include "FixedKinematicPrimitive.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

class FixedKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : BlockBase() {}

  public slots:
    void setOffsetHookToPivot( Eigen::Vector3d offset ) {
      hookToPivot.setOffset( offset );
    }
    void setOffsetPivotToTow( Eigen::Vector3d offset ) {
      pivotToTow.setOffset( offset );
    }

    void setPose( const Point_3 position, const Eigen::Quaterniond orientation, const PoseOption::Options options ) {
      hookToPivot.setPose( position, orientation, options );
      pivotToTow.setPose( hookToPivot.positionCalculated, hookToPivot.orientation, options );

      emit poseHookPointChanged( position, orientation, options );
      emit posePivotPointChanged( hookToPivot.positionCalculated, hookToPivot.orientation, options );
      emit poseTowPointChanged( pivotToTow.positionCalculated, pivotToTow.orientation, options );
    }

  signals:
    void poseHookPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );
    void posePivotPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );
    void poseTowPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );

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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new FixedKinematic;
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Offset Hook to Pivot" ), QLatin1String( SLOT( setOffsetHookToPivot( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Offset Pivot To Tow" ), QLatin1String( SLOT( setOffsetPivotToTow( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }
};
