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

// general stanley-controller:
// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

// reverse driving:
// https://www.researchgate.net/publication/339107638_Design_of_a_transverse_controller_for_an_autonomous_driving_model_car_based_on_the_Stanley_approach

class StanleyGuidance : public BlockBase {
  Q_OBJECT

public:
  explicit StanleyGuidance() : BlockBase() {}

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void
  setPoseFrontWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options );
  void
  setPoseRearWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options );

  void setHeadingOfPathFrontWheels( NUMBER_SIGNATURE_SLOT );
  void setHeadingOfPathRearWheels( NUMBER_SIGNATURE_SLOT );

  void setXteFrontWheels( const double distance, const CalculationOption::Options options = CalculationOption::Option::None );
  void setXteRearWheels( const double distance, const CalculationOption::Options options = CalculationOption::Option::None );

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
  StanleyGuidanceFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Stanley Path Follower" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
