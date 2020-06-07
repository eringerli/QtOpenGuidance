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

#include <QQuaternion>
#include <QVector3D>

#include "BlockBase.h"

#include "qneblock.h"
#include "qneport.h"

#include "../kinematic/cgalKernel.h"
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

  public slots:
    void setSteeringAngle( double steeringAngle ) {
      steeringAngle2Ago = steeringAngle1Ago;
      steeringAngle1Ago = this->steeringAngle;
      this->steeringAngle = steeringAngle;
    }

    void setPoseFrontWheels( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->positionFrontWheels = position;
        this->orientation1AgoFrontWheels = this->orientationFrontWheels;
        this->orientationFrontWheels = orientation;
      }
    }

    void setPoseRearWheels( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->positionRearWheels = position;
        this->orientation1AgoRearWheels = this->orientationRearWheels;
        this->orientationRearWheels = orientation;
      }
    }

    void setHeadingOfPathFrontWheels( double headingOfPath ) {
      this->headingOfPathRadiansFrontWheels = qDegreesToRadians( headingOfPath );
    }

    void setHeadingOfPathRearWheels( double headingOfPath ) {
      this->headingOfPathRadiansRearWheels = qDegreesToRadians( headingOfPath );
    }

    void setXteFrontWheels( double distance ) {
      if( !qIsInf( distance ) && velocity >= 0 ) {
        double stanleyYawCompensation = /*normalizeAngle*/( ( headingOfPathRadiansFrontWheels ) - ( qDegreesToRadians( double( orientationFrontWheels.toEulerAngles().z() ) ) ) );
        double stanleyXteCompensation = atan( ( stanleyGainKForwards * -distance ) / ( velocity + stanleyGainKSoftForwards ) );
        double stanleyYawDampening = /*normalizeAngle*/( stanleyGainDampeningYawForwards *
            ( qDegreesToRadians( normalizeAngleDegrees( double( this->orientation1AgoFrontWheels.toEulerAngles().z() ) ) - normalizeAngleDegrees( double( this->orientationFrontWheels.toEulerAngles().z() ) ) ) -
              ( yawTrajectory1AgoFrontWheels - headingOfPathRadiansFrontWheels ) ) );
        double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringForwards * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
        double steerAngleRequested = qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

        if( steerAngleRequested >= maxSteeringAngle ) {
          steerAngleRequested = maxSteeringAngle;
        }

        if( steerAngleRequested <= -maxSteeringAngle ) {
          steerAngleRequested = -maxSteeringAngle;
        }

//        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadians ) << normalizeAngleRadians( qDegreesToRadians( orientation.toEulerAngles().z() ) );

        emit steerAngleChanged( steerAngleRequested );
        yawTrajectory1AgoFrontWheels = headingOfPathRadiansFrontWheels;
      }
    }

    void setXteRearWheels( double distance ) {
      if( !qIsInf( distance ) && velocity < 0 ) {
        double stanleyYawCompensation = /*normalizeAngle*/( ( headingOfPathRadiansRearWheels ) - ( qDegreesToRadians( double( orientationRearWheels.toEulerAngles().z() ) ) ) );
        double stanleyXteCompensation = atan( ( stanleyGainKReverse * -distance ) / ( velocity + -stanleyGainKSoftReverse ) );
        double stanleyYawDampening = /*normalizeAngle*/( stanleyGainDampeningYawReverse *
            ( qDegreesToRadians( normalizeAngleDegrees( double( this->orientation1AgoRearWheels.toEulerAngles().z() ) ) - normalizeAngleDegrees( double( this->orientationRearWheels.toEulerAngles().z() ) ) ) -
              ( yawTrajectory1AgoRearWheels - headingOfPathRadiansRearWheels ) ) );
        double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteeringReverse * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
        double steerAngleRequested = -qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

        if( steerAngleRequested >= maxSteeringAngle ) {
          steerAngleRequested = maxSteeringAngle;
        }

        if( steerAngleRequested <= -maxSteeringAngle ) {
          steerAngleRequested = -maxSteeringAngle;
        }

//        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadiansRearWheels ) << normalizeAngleRadians( qDegreesToRadians( orientationRearWheels.toEulerAngles().z() ) );

        emit steerAngleChanged( steerAngleRequested );
        yawTrajectory1AgoRearWheels = headingOfPathRadiansRearWheels;
      }
    }

    void setStanleyGainKForwards( double stanleyGain ) {
      this->stanleyGainKForwards = stanleyGain;
    }
    void setStanleyGainKSoftForwards( double stanleyGain ) {
      this->stanleyGainKSoftForwards = stanleyGain;
    }
    void setStanleyGainDampeningYawForwards( double stanleyGain ) {
      this->stanleyGainDampeningYawForwards = stanleyGain;
    }
    void setStanleyGainDampeningSteeringForwards( double stanleyGain ) {
      this->stanleyGainDampeningSteeringForwards = stanleyGain;
    }

    void setStanleyGainKReverse( double stanleyGain ) {
      this->stanleyGainKReverse = stanleyGain;
    }
    void setStanleyGainKSoftReverse( double stanleyGain ) {
      this->stanleyGainKSoftReverse = stanleyGain;
    }
    void setStanleyGainDampeningYawReverse( double stanleyGain ) {
      this->stanleyGainDampeningYawReverse = stanleyGain;
    }
    void setStanleyGainDampeningSteeringReverse( double stanleyGain ) {
      this->stanleyGainDampeningSteeringReverse = stanleyGain;
    }

    void setVelocity( double velocity ) {
      this->velocity = velocity;
    }

    void setMaxSteeringAngle( double maxSteeringAngle ) {
      this->maxSteeringAngle = maxSteeringAngle;
    }

    void emitConfigSignals() override {
      emit steerAngleChanged( 0 );
    }

  signals:
    void steerAngleChanged( double );

  private:

  public:
    Point_3 positionFrontWheels = Point_3( 0, 0, 0 );
    QQuaternion orientationFrontWheels = QQuaternion();
    QQuaternion orientation1AgoFrontWheels = QQuaternion();

    Point_3 positionRearWheels = Point_3( 0, 0, 0 );
    QQuaternion orientationRearWheels = QQuaternion();
    QQuaternion orientation1AgoRearWheels = QQuaternion();

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

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new StanleyGuidance();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Pose Front Wheels" ), QLatin1String( SLOT( setPoseFrontWheels( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Pose Rear Wheels" ), QLatin1String( SLOT( setPoseRearWheels( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );

      b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( double ) ) ) );

      b->addInputPort( QStringLiteral( "XTE Front Wheels" ), QLatin1String( SLOT( setXteFrontWheels( double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path Front Wheels" ), QLatin1String( SLOT( setHeadingOfPathFrontWheels( double ) ) ) );
      b->addInputPort( QStringLiteral( "XTE Rear Wheels" ), QLatin1String( SLOT( setXteRearWheels( double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path Rear Wheels" ), QLatin1String( SLOT( setHeadingOfPathRearWheels( double ) ) ) );

      b->addInputPort( QStringLiteral( "Stanley Gain K Forwards" ), QLatin1String( SLOT( setStanleyGainKForwards( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening Forwards" ), QLatin1String( SLOT( setStanleyGainKSoftForwards( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningYawForwards( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Forwards" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringForwards( double ) ) ) );

      b->addInputPort( QStringLiteral( "Stanley Gain K Reverse" ), QLatin1String( SLOT( setStanleyGainKReverse( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening Reverse" ), QLatin1String( SLOT( setStanleyGainKSoftReverse( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningYawReverse( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening Reverse" ), QLatin1String( SLOT( setStanleyGainDampeningSteeringReverse( double ) ) ) );

      b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( double ) ) ) );

      b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( double ) ) ) );

      return b;
    }
};
