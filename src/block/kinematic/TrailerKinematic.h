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
#include "TrailerKinematicPrimitive.h"

#include "helpers/eigenHelper.h"

class TrailerKinematic : public BlockBase {
  Q_OBJECT

public:
  explicit TrailerKinematic() : BlockBase() {}

public Q_SLOTS:
  void setOffsetHookToPivot( const Eigen::Vector3d& offset );
  void setOffsetPivotToTow( const Eigen::Vector3d& offset );
  void setMaxJackknifeAngle( NUMBER_SIGNATURE_SLOT );
  void setMaxAngle( NUMBER_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseHookPointChanged( POSE_SIGNATURE_SIGNAL );
  void posePivotPointChanged( POSE_SIGNATURE_SIGNAL );
  void poseTowPointChanged( POSE_SIGNATURE_SIGNAL );

private:
  TrailerKinematicPrimitive hookToPivot;
  FixedKinematicPrimitive   pivotToTow;
};

class TrailerKinematicFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Trailer Kinematic" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
