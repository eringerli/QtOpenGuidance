// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"
#include "helpers/signatures.h"

// general stanley-controller:
// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

// reverse driving:
// https://www.researchgate.net/publication/339107638_Design_of_a_transverse_controller_for_an_autonomous_driving_model_car_based_on_the_Stanley_approach

class StanleyGuidance : public BlockBase {
  Q_OBJECT

public:
  explicit StanleyGuidance( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void setPoseFrontWheels( POSE_SIGNATURE_SLOT );
  void setPoseRearWheels( POSE_SIGNATURE_SLOT );

  void setHeadingOfPathFrontWheels( NUMBER_SIGNATURE_SLOT );
  void setHeadingOfPathRearWheels( NUMBER_SIGNATURE_SLOT );

  void setXteFrontWheels( NUMBER_SIGNATURE_SLOT );
  void setXteRearWheels( NUMBER_SIGNATURE_SLOT );

  void setStanleyGainKForwards( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainKSoftForwards( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainDampeningYawForwards( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainDampeningSteeringForwards( NUMBER_SIGNATURE_SLOT );

  void setStanleyGainKReverse( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainKSoftReverse( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainDampeningYawReverse( NUMBER_SIGNATURE_SLOT );
  void setStanleyGainDampeningSteeringReverse( NUMBER_SIGNATURE_SLOT );

  void setVelocity( NUMBER_SIGNATURE_SLOT );

  void setMaxSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void emitConfigSignals() override;

Q_SIGNALS:
  void steerAngleChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  Eigen::Vector3d    positionFrontWheels        = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientationFrontWheels     = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Quaterniond orientation1AgoFrontWheels = Eigen::Quaterniond( 0, 0, 0, 0 );

  Eigen::Vector3d    positionRearWheels        = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientationRearWheels     = Eigen::Quaterniond( 0, 0, 0, 0 );
  Eigen::Quaterniond orientation1AgoRearWheels = Eigen::Quaterniond( 0, 0, 0, 0 );

  double velocity                        = 0;
  double headingOfPathRadiansFrontWheels = 0;
  double headingOfPathRadiansRearWheels  = 0;

  double stanleyGainKForwards                 = 1;
  double stanleyGainKSoftForwards             = 1;
  double stanleyGainDampeningYawForwards      = 0;
  double stanleyGainDampeningSteeringForwards = 0;

  double stanleyGainKReverse                 = 1;
  double stanleyGainKSoftReverse             = 1;
  double stanleyGainDampeningYawReverse      = 0;
  double stanleyGainDampeningSteeringReverse = 0;

  double maxSteeringAngle = 40;

  double steeringAngle     = 0;
  double steeringAngle1Ago = 0;
  double steeringAngle2Ago = 0;

  double yawTrajectory1AgoFrontWheels = 0;
  double yawTrajectory1AgoRearWheels  = 0;
};

class StanleyGuidanceFactory : public BlockFactory {
  Q_OBJECT

public:
  StanleyGuidanceFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "Stanley Path Follower" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Guidance" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
