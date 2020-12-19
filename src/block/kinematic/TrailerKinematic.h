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
#include <QtMath>
#include <QtDebug>

#include "block/BlockBase.h"

#include "FixedKinematicPrimitive.h"
#include "TrailerKinematicPrimitive.h"

#include "helpers/eigenHelper.h"
#include "kinematic/PoseOptions.h"

class TrailerKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit TrailerKinematic()
      : BlockBase() {}

  public Q_SLOTS:
    void setOffsetHookToPivot( const Eigen::Vector3d& offset ) {
      hookToPivot.setOffset( offset );
    }
    void setOffsetPivotToTow( const Eigen::Vector3d& offset ) {
      pivotToTow.setOffset( offset );
    }
    void setMaxJackknifeAngle( const double maxAngle ) {
      hookToPivot.setMaxJackknifeAngle( maxAngle );
    }
    void setMaxAngle( const double maxAngle ) {
      hookToPivot.setMaxAngle( maxAngle );
    }

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
      hookToPivot.setPose( position, orientation, options );
      pivotToTow.setPose( hookToPivot.positionCalculated, hookToPivot.orientation, options );

      Q_EMIT poseHookPointChanged( position, orientation, options );
      Q_EMIT posePivotPointChanged( hookToPivot.positionCalculated, hookToPivot.orientation, options );
      Q_EMIT poseTowPointChanged( pivotToTow.positionCalculated, pivotToTow.orientation, options );
    }

  Q_SIGNALS:
    void poseHookPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void posePivotPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );
    void poseTowPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

  private:
    TrailerKinematicPrimitive hookToPivot;
    FixedKinematicPrimitive pivotToTow;
};

class TrailerKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    TrailerKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Kinematic" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new TrailerKinematic();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Offset Hook to Pivot" ), QLatin1String( SLOT( setOffsetHookToPivot( const Eigen::Vector3d& ) ) ) );
      b->addInputPort( QStringLiteral( "Offset Pivot To Tow" ), QLatin1String( SLOT( setOffsetPivotToTow( const Eigen::Vector3d& ) ) ) );
      b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( const double ) ) ) );
      b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      return b;
    }
};
