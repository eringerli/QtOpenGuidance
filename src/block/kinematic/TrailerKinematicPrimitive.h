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
  explicit TrailerKinematicPrimitive( const BlockBaseId          idHint,
                                      const bool                 systemBlock,
                                      const QString              type,
                                      const BlockBase::TypeColor typeColor )
      : BlockBase( idHint, systemBlock, type, typeColor ) {}

public Q_SLOTS:
  void setOffset( VECTOR_SIGNATURE_SLOT );
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

  FixedKinematicPrimitive fixedKinematic = FixedKinematicPrimitive( 0, false, "", BlockBase::TypeColor::System );
};

class TrailerKinematicPrimitiveFactory : public BlockFactory {
  Q_OBJECT

public:
  TrailerKinematicPrimitiveFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = BlockBase::TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Trailer Kinematic Primitive" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Calculations" ); }

  virtual std::unique_ptr< BlockBase > createBlock( const BlockBaseId idHint = 0 ) override;
};
