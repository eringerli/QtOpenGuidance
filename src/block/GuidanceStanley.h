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

#ifndef STANLEYGUIDANCE_H
#define STANLEYGUIDANCE_H

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
    void setSteeringAngle( float steeringAngle ) {
      steeringAngle2Ago = steeringAngle1Ago;
      steeringAngle1Ago = this->steeringAngle;
      this->steeringAngle = double( steeringAngle );
    }

    void setPose( const Point_3& position, QQuaternion orientation, PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        this->position = position;
        this->orientation1Ago = this->orientation;
        this->orientation = orientation;
      }
    }

    void setHeadingOfPath( float headingOfPath ) {
      this->headingOfPath = double( headingOfPath );
    }

    void setXte( float distance ) {
      if( !qIsInf( distance ) ) {
        double stanleyYawCompensation = /*normalizeAngle*/( ( headingOfPath ) - ( qDegreesToRadians( double( orientation.toEulerAngles().z() ) ) ) );
        double stanleyXteCompensation = atan( ( stanleyGainK * double( -distance ) ) / ( double( velocity ) + stanleyGainKSoft ) );
        double stanleyYawDampening = /*normalizeAngle*/( stanleyGainDampeningYaw *
            ( qDegreesToRadians( normalizeAngleDegrees( double( this->orientation1Ago.toEulerAngles().z() ) ) - normalizeAngleDegrees( double( this->orientation.toEulerAngles().z() ) ) ) -
              ( yawTrajectory1Ago - headingOfPath ) ) );
        double stanleySteeringDampening = /*normalizeAngle*/( stanleyGainDampeningSteering * qDegreesToRadians( steeringAngle1Ago - steeringAngle ) );
        double steerAngleRequested = qRadiansToDegrees( normalizeAngleRadians( stanleyYawCompensation + stanleyXteCompensation + stanleyYawDampening + stanleySteeringDampening ) );

        if( steerAngleRequested >= maxSteeringAngle ) {
          steerAngleRequested = maxSteeringAngle;
        }

        if( steerAngleRequested <= -maxSteeringAngle ) {
          steerAngleRequested = -maxSteeringAngle;
        }

//        qDebug() << fixed << forcesign << qSetRealNumberPrecision( 4 ) << stanleyYawCompensation << stanleyXteCompensation << stanleyYawDampening << stanleySteeringDampening << steerAngleRequested << normalizeAngleRadians( headingOfABLine ) << normalizeAngleRadians( qDegreesToRadians( orientation.toEulerAngles().z() ) );

        emit steerAngleChanged( float( steerAngleRequested ) );
        yawTrajectory1Ago = headingOfPath;
      }
    }

    void setStanleyGainK( float stanleyGain ) {
      this->stanleyGainK = double( stanleyGain );
    }
    void setStanleyGainKSoft( float stanleyGain ) {
      this->stanleyGainKSoft = double( stanleyGain );
    }
    void setStanleyGainDampeningYaw( float stanleyGain ) {
      this->stanleyGainDampeningYaw = double( stanleyGain );
    }
    void setStanleyGainDampeningSteering( float stanleyGain ) {
      this->stanleyGainDampeningSteering = double( stanleyGain );
    }

    void setVelocity( float velocity ) {
      this->velocity = double( velocity );
    }

    void setMaxSteeringAngle( float maxSteeringAngle ) {
      this->maxSteeringAngle = double( maxSteeringAngle );
    }

    void emitConfigSignals() override {
      emit steerAngleChanged( 0 );
    }

  signals:
    void steerAngleChanged( float );

  private:

    double normalizeAngleRadians( double angle ) {
      while( angle > M_PI ) {
        angle -= M_PI * 2;
      }

      while( angle < -M_PI ) {
        angle += M_PI * 2;
      }

      return angle;
    }

    double normalizeAngleDegrees( double angle ) {
      while( angle > 180 ) {
        angle -= 360;
      }

      while( angle < -180 ) {
        angle += 360;
      }

      return angle;
    }

    // https://stackoverflow.com/a/4448097

    // Compute the dot product AB . BC
    double dotProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cX - bX ) + ( bY - aY ) * ( cY - bY );
    }

    // Compute the cross product AB x AC
    double crossProduct( double aX, double aY, double bX, double bY, double cX, double cY ) {
      return ( bX - aX ) * ( cY - aY ) - ( bY - aY ) * ( cX - aX );
    }

    // Compute the distance from A to B
    double distance2D( double aX, double aY, double bX, double bY ) {
      double d1 = aX - bX;
      double d2 = aY - bY;

      return qSqrt( d1 * d1 + d2 * d2 );
    }

    // Compute the distance from AB to C
    // if isSegment is true, AB is a segment, not a line.
    // if <0: left side of line
    double lineToPointDistance2D( double aX, double aY, double bX, double bY, double cX, double cY, bool isSegment ) {
      double dist = crossProduct( aX, aY, bX, bY, cX, cY ) / distance2D( aX, aY, bX, bY );

      if( isSegment ) {
        if( dotProduct( aX, aY, bX, bY, cX, cY ) > 0 ) {
          return distance2D( bX, bY, cX, cY );
        }

        if( dotProduct( bX, bY, aX, aY, cX, cY ) > 0 ) {
          return distance2D( aX, aY, cX, cY );
        }
      }

      return dist;
    }

  public:
    Point_3 position = Point_3();
    QQuaternion orientation = QQuaternion();
    QQuaternion orientation1Ago = QQuaternion();
    double velocity = 0;
    double headingOfPath = 0;
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

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new StanleyGuidance();
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3&, const QQuaternion, const PoseOption::Options ) ) ) );
      b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( float ) ) ) );
      b->addInputPort( QStringLiteral( "Velocity" ), QLatin1String( SLOT( setVelocity( float ) ) ) );
      b->addInputPort( QStringLiteral( "XTE" ), QLatin1String( SLOT( setXte( float ) ) ) );
      b->addInputPort( QStringLiteral( "Heading of Path" ), QLatin1String( SLOT( setHeadingOfPath( float ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K" ), QLatin1String( SLOT( setStanleyGainK( float ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain K Softening" ), QLatin1String( SLOT( setStanleyGainKSoft( float ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Yaw Dampening" ), QLatin1String( SLOT( setStanleyGainDampeningYaw( float ) ) ) );
      b->addInputPort( QStringLiteral( "Stanley Gain Steering Dampening" ), QLatin1String( SLOT( setStanleyGainDampeningSteering( float ) ) ) );
      b->addInputPort( QStringLiteral( "Max Steering Angle" ), QLatin1String( SLOT( setMaxSteeringAngle( float ) ) ) );

      b->addOutputPort( QStringLiteral( "Steer Angle" ), QLatin1String( SIGNAL( steerAngleChanged( float ) ) ) );

      return b;
    }
};

#endif // STANLEYGUIDANCE_H

