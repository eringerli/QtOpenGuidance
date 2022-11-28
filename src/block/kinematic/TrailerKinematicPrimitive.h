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

#include "helpers/anglesHelper.h"

class TrailerKinematicPrimitive : public BlockBase {
  Q_OBJECT

public:
  explicit TrailerKinematicPrimitive() : BlockBase() {}

public Q_SLOTS:
  void setOffset( const Eigen::Vector3d& offset );
  void setMaxJackknifeAngle( NUMBER_SIGNATURE_SLOT );
  void setMaxAngle( NUMBER_SIGNATURE_SLOT );

  void setPose( POSE_SIGNATURE_SLOT );
  void setPoseInitialMpcPivot( POSE_SIGNATURE_SLOT );

Q_SIGNALS:
  void poseChanged( POSE_SIGNATURE_SIGNAL );

public:
  Eigen::Vector3d    positionCalculated    = Eigen::Vector3d( -1, 0, 0 );
  Eigen::Quaterniond orientationCalculated = Eigen::Quaterniond( 0, 0, 0, 0 );

  Eigen::Vector3d lastPosition = Eigen::Vector3d( 0, 0, 0 );

  double maxJackknifeAngleRad = degreesToRadians( double( 120 ) );
  double maxAngleRad          = degreesToRadians( double( 150 ) );

  Eigen::Vector3d    positionMpcInitialPivot    = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientationMpcInitialPivot = Eigen::Quaterniond( 0, 0, 0, 0 );

  FixedKinematicPrimitive fixedKinematic;
};

class TrailerKinematicPrimitiveFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicPrimitiveFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Trailer Kinematic Primitive" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
