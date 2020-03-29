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

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"
#include "../kinematic/PathPrimitive.h"

#include <QVector>
#include <QSharedPointer>

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

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

    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation1Ago = this->orientation;
        this->orientation = orientation;
      }
    }

    void setHeadingOfPath( double headingOfPath ) {
      this->headingOfPathRadians = qDegreesToRadians( headingOfPath );
    }

    void setXte( double distance ) {
      if( !qIsInf( distance ) ) {
        double stanleyYawCompensation = /*normalizeAngle*/( ( headingOfPathRadians ) - ( qDegreesToRadians( double( orientation.toEulerAngles().z() ) ) ) );
        double stanleyXteCompensation = atan( ( stanleyGainK * -distance ) / ( velocity + stanleyGainKSoft ) );
        double stanleyYawDampening = /*normalizeAngle*/( stanleyGainDampeningYaw *
            ( qDegreesToRadians( normalizeAngleDegrees( double( this->orientation1Ago.toEulerAngles().z() ) ) - normalizeAngleDegrees( double( this->orientation.toEulerAngles().z() ) ) ) -
              ( yawTrajectory1Ago - headingOfPathRadians ) ) );
        double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteering * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
        double steerAngleRequested = qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

        if( steerAngleRequested >= maxSteeringAngle ) {
          steerAngleRequested = maxSteeringAngle;
        }

        if( steerAngleRequested <= -maxSteeringAngle ) {
          steerAngleRequested = -maxSteeringAngle;
        }

//        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfPathRadians ) << normalizeAngleRadians( qDegreesToRadians( orientation.toEulerAngles().z() ) );

        emit steerAngleChanged( steerAngleRequested );
        yawTrajectory1Ago = headingOfPathRadians;
      }
    }

    void setStanleyGainK( double stanleyGain ) {
      this->stanleyGainK = stanleyGain;
    }
    void setStanleyGainKSoft( double stanleyGain ) {
      this->stanleyGainKSoft = stanleyGain;
    }
    void setStanleyGainDampeningYaw( double stanleyGain ) {
      this->stanleyGainDampeningYaw = stanleyGain;
    }
    void setStanleyGainDampeningSteering( double stanleyGain ) {
      this->stanleyGainDampeningSteering = stanleyGain;
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
    Point_3 position = Point_3( 0, 0, 0 );
    QQuaternion orientation = QQuaternion();
    QQuaternion orientation1Ago = QQuaternion();
    double velocity = 0;
    double headingOfPathRadians = 0;
    double distance = 0;

    double stanleyGainK = 1;
    double stanleyGainKSoft = 1;
    double stanleyGainDampeningYaw = 1;
    double stanleyGainDampeningSteering = 1;
    double maxSteeringAngle = 40;

    double steeringAngle = 0;
    double steeringAngle1Ago = 0;
    double steeringAngle2Ago = 0;

    double yawTrajectory1Ago = 0;
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

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( double ) ) ) );
      b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( double ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SLOT( setHeadingOfPath( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K" ), QLatin1String( SLOT( setStanleyGainK( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening" ), QLatin1String( SLOT( setStanleyGainKSoft( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening" ), QLatin1String( SLOT( setStanleyGainDampeningYaw( double ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening" ), QLatin1String( SLOT( setStanleyGainDampeningSteering( double ) ) ) );
      b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( double ) ) ) );

      b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( double ) ) ) );

      return b;
    }
};
