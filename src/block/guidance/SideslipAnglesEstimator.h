// Copyright( C ) 2022 Christian Riggenbach
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
#include <memory>

#include "block/BlockBase.h"

#include "helpers/eigenHelper.h"

#include "filter/SideslipAngleObserver/SideslipAnglesObserver.h"

// general stanley-controller:
// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

// reverse driving:
// https://www.researchgate.net/publication/339107638_Design_of_a_transverse_controller_for_an_autonomous_driving_model_car_based_on_the_Stanley_approach

class SideslipAnglesEstimator : public BlockBase {
  Q_OBJECT

public:
  explicit SideslipAnglesEstimator() : BlockBase() {}

public Q_SLOTS:
  void setSteeringAngle( NUMBER_SIGNATURE_SLOT );

  void
  setPoseRearWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const CalculationOption::Options options );

  void setHeadingOfPathRearWheels( NUMBER_SIGNATURE_SLOT );

  void setXteRearWheels( NUMBER_SIGNATURE_SLOT );
  void setCurvature( NUMBER_SIGNATURE_SLOT );

  void setVelocity( NUMBER_SIGNATURE_SLOT );

  void emitConfigSignals() override;

Q_SIGNALS:
  void sideslipAngleFrontChanged( NUMBER_SIGNATURE_SIGNAL );
  void sideslipAngleRearChanged( NUMBER_SIGNATURE_SIGNAL );

private:
  Eigen::Vector3d    positionRearWheels    = Eigen::Vector3d( 0, 0, 0 );
  Eigen::Quaterniond orientationRearWheels = Eigen::Quaterniond( 0, 0, 0, 0 );

  double velocity                       = 0;
  double headingOfPathRadiansRearWheels = 0;
  double curvature                      = 0;

  double steeringAngle = 0;

  std::unique_ptr< SideslipAnglesObserver > sideslipAnglesObserver = std::make_unique< SideslipAnglesObserver >();
};

class SideslipAnglesEstimatorFactory : public BlockFactory {
  Q_OBJECT

public:
  SideslipAnglesEstimatorFactory( QThread* thread ) : BlockFactory( thread ) {}

  QString getNameOfFactory() override { return QStringLiteral( "Sideslip Angles Estimator" ); }

  QString getCategoryOfFactory() override { return QStringLiteral( "Guidance" ); }

  virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override;
};
