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

#include "../BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../helpers/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>

// general stanley-controller:
// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

// reverse driving:
// https://www.researchgate.net/publication/339107638_Design_of_a_transverse_controller_for_an_autonomous_driving_model_car_based_on_the_Stanley_approach

class StanleyGuidance : public BlockBase {
    Q_OBJECT

  public:
    explicit StanleyGuidance()
      : BlockBase() {}

  public Q_SLOTS:
    void setSteeringAngle( const double steeringAngle ) {
      steeringAngle2Ago = steeringAngle1Ago;
      steeringAngle1Ago = this->steeringAngle;
      this->steeringAngle = steeringAngle;
    }

    void setPoseFrontWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->positionFrontWheels = position;
        this->orientation1AgoFrontWheels = this->orientationFrontWheels;
        this->orientationFrontWheels = orientation;
      }
    }

    void setPoseRearWheels( const Eigen::Vector3d& position, const Eigen::Quaterniond& orientation, const PoseOption::Options& options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->positionRearWheels = position;
        this->orientation1AgoRearWheels = this->orientationRearWheels;
        this->orientationRearWheels = orientation;
      }
    }

    void setHeadingOfPathFrontWheels( const double headingOfPath ) {
      this->headingOfPathRadiansFrontWheels = qDegreesToRadians( headingOfPath );
    }

    void setHeadingOfPathRearWheels( const double headingOfPath ) {
      this->headingOfPathRadiansRearWheels = qDegreesToRadians( headingOfPath );
    }

    void setXteFrontWheels( const double distance );

    void setXteRearWheels( const double distance );

    void setStanleyGainKForwards( const double stanleyGain ) {
      this->stanleyGainKForwards = stanleyGain;
    }
    void setStanleyGainKSoftForwards( const double stanleyGain ) {
      this->stanleyGainKSoftForwards = stanleyGain;
    }
    void setStanleyGainDampeningYawForwards( const double stanleyGain ) {
      this->stanleyGainDampeningYawForwards = stanleyGain;
    }
    void setStanleyGainDampeningSteeringForwards( const double stanleyGain ) {
      this->stanleyGainDampeningSteeringForwards = stanleyGain;
    }

    void setStanleyGainKReverse( const double stanleyGain ) {
      this->stanleyGainKReverse = stanleyGain;
    }
    void setStanleyGainKSoftReverse( const double stanleyGain ) {
      this->stanleyGainKSoftReverse = stanleyGain;
    }
    void setStanleyGainDampeningYawReverse( const double stanleyGain ) {
      this->stanleyGainDampeningYawReverse = stanleyGain;
    }
    void setStanleyGainDampeningSteeringReverse( const double stanleyGain ) {
      this->stanleyGainDampeningSteeringReverse = stanleyGain;
    }

    void setVelocity( const double velocity ) {
      this->velocity = velocity;
    }

    void setMaxSteeringAngle( const double maxSteeringAngle ) {
      this->maxSteeringAngle = maxSteeringAngle;
    }

    void emitConfigSignals() override {
      Q_EMIT steerAngleChanged( 0 );
    }

  Q_SIGNALS:
    void steerAngleChanged( const double );

  private:

  public:
    Eigen::Vector3d positionFrontWheels = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientationFrontWheels = Eigen::Quaterniond();
    Eigen::Quaterniond orientation1AgoFrontWheels = Eigen::Quaterniond();

    Eigen::Vector3d positionRearWheels = Eigen::Vector3d( 0, 0, 0 );
    Eigen::Quaterniond orientationRearWheels = Eigen::Quaterniond();
    Eigen::Quaterniond orientation1AgoRearWheels = Eigen::Quaterniond();

    double velocity = 0;
    double headingOfPathRadiansFrontWheels = 0;
    double headingOfPathRadiansRearWheels = 0;

    double stanleyGainKForwards = 1;
    double stanleyGainKSoftForwards = 1;
    double stanleyGainDampeningYawForwards = 0;
    double stanleyGainDampeningSteeringForwards = 0;

    double stanleyGainKReverse = 1;
    double stanleyGainKSoftReverse = 1;
    double stanleyGainDampeningYawReverse = 0;
    double stanleyGainDampeningSteeringReverse = 0;

    double maxSteeringAngle = 40;

    double steeringAngle = 0;
    double steeringAngle1Ago = 0;
    double steeringAngle2Ago = 0;

    double yawTrajectory1AgoFrontWheels = 0;
    double yawTrajectory1AgoRearWheels = 0;
};

class StanleyGuidanceFactory : public BlockFactory {
    Q_OBJECT

  public:
    StanleyGuidanceFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Stanley Path Follower" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Guidance" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new StanleyGuidance();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose Front Wheels" ), QLatin1String( SLOT( setPoseFrontWheels( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Rear Wheels" ), QLatin1String( SLOT( setPoseRearWheels( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( const double ) ) ) );

      b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( const double ) ) ) );

      b->addInputPort( QStringLiteral( "XTE Front Wheels" ), QLatin1String( SLOT( setXteFrontWheels( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path Front Wheels" ), QLatin1String( SLOT( setHeadingOfPathFrontWheels( const double ) ) ) );
      b->addInputPort( QStringLiteral( "XTE Rear Wheels" ), QLatin1String( SLOT( setXteRearWheels( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ), QLatin1String( SLOT( setHeadingOfPathRearWheels( const double ) ) ) );

      b->addInputPort( QStringLiteral( "Stanley Gain K Forwards" ), QLatin1String( SLOT( setStanleyGainKForwards( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening Forwards" ), QLatin1String( SLOT( setStanleyGainKSoftForwards( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningYawForwards( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringForwards( const double ) ) ) );

      b->addInputPort( QStringLiteral( "Stanley Gain K Reverse" ), QLatin1String( SLOT( setStanleyGainKReverse( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening Reverse" ), QLatin1String( SLOT( setStanleyGainKSoftReverse( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningYawReverse( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringReverse( const double ) ) ) );

      b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( const double ) ) ) );

      b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( const double ) ) ) );

      return b;
    }
};
