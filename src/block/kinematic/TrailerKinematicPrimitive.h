// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

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
  TrailerKinematicPrimitiveFactory( QThread* thread ) : BlockFactory( thread, false ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Trailer Kinematic Primitive" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Calculations" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id = 0 ) override;
};
