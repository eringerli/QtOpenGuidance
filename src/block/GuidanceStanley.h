// Copyright( C ) 2019 Christian Riggenbach
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

#include "../kinematic/Tile.h"

#include <QVector>
#include <QSharedPointer>
#include "PathPrimitive.h"

// http://ai.stanford.edu/~gabeh/papers/hoffmann_stanley_control07.pdf
// https://github.com/AtsushiSakai/PythonRobotics/blob/master/PathTracking/stanley_controller/stanley_controller.py

class StanleyGuidance : public BlockBase {
    Q_OBJECT

  public:
    explicit StanleyGuidance( Tile* tile )
      : BlockBase() {
      this->tile = tile->getTileForOffset( 0, 0 );
    }

  public slots:
    void setSteeringAngle( float steeringAngle ) {
      steeringAngle2Ago = steeringAngle1Ago;
      steeringAngle1Ago = this->steeringAngle;
      this->steeringAngle = double( steeringAngle );
    }

    void setPose( Tile* tile, QVector3D position, QQuaternion orientation ) {

      double distance = qInf();
      double headingOfABLine = 0;

      for( auto primitive : plan ) {
        PathPrimitiveLine* line =  qobject_cast<PathPrimitiveLine*>( primitive.data() );

        if( line ) {
          double distanceTmp = lineToPointDistance2D(
                                 line->x1, line->y1,
                                 line->x2, line->y2,
                                 tile->x + double( position.x() ), tile->y + double( position.y() ),
                                 line->segment
                               );

          if( distanceTmp < distance ) {
            headingOfABLine = qAtan2( line->y1 - line->y2, line->x1 - line->x2 ) - M_PI;
            distance = distanceTmp;
          }
        }
      }

      if( !qIsInf( distance ) ) {
        emit xteChanged( float( distance ) );
      } else {
        emit xteChanged( 0 );
      }

      if( !qIsInf( distance ) ) {
        double stanleyYawCompensation = /*normalizeAngle*/( ( headingOfABLine ) - ( qDegreesToRadians( double( orientation.toEulerAngles().z() ) ) ) );
        double stanleyXteCompensation = atan( ( stanleyGainK * -distance ) / ( double( velocity ) + stanleyGainKSoft ) );
        double stanleyYawDampening = /*normalizeAngle*/( stanleyGainDampeningYaw *
            ( qDegreesToRadians( normalizeAngleDegrees( double( this->orientation.toEulerAngles().z() ) ) - normalizeAngleDegrees( double( orientation.toEulerAngles().z() ) ) ) -
              ( yawTrajectory1Ago - headingOfABLine ) ) );
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

        this->tile = tile;
        this->position = position;
        this->orientation = orientation;
        yawTrajectory1Ago = headingOfABLine;
      }

    }

    void setPlan( QVector<QSharedPointer<PathPrimitive>> plan ) {
      this->plan = plan;
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
      emit xteChanged( 0 );
      emit steerAngleChanged( 0 );
    }

  signals:
    void xteChanged( float );
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
    double distance( double aX, double aY, double bX, double bY ) {
      double d1 = aX - bX;
      double d2 = aY - bY;

      return qSqrt( d1 * d1 + d2 * d2 );
    }

    // Compute the distance from AB to C
    // if isSegment is true, AB is a segment, not a line.
    // if <0: left side of line
    double lineToPointDistance2D( double aX, double aY, double bX, double bY, double cX, double cY, bool isSegment ) {
      double dist = crossProduct( aX, aY, bX, bY, cX, cY ) / distance( aX, aY, bX, bY );

      if( isSegment ) {
        if( dotProduct( aX, aY, bX, bY, cX, cY ) > 0 ) {
          return distance( bX, bY, cX, cY );
        }

        if( dotProduct( bX, bY, aX, aY, cX, cY ) > 0 ) {
          return distance( aX, aY, cX, cY );
        }
      }

      return dist;
    }

  public:
    Tile* tile = nullptr;
    QVector3D position = QVector3D();
    QQuaternion orientation = QQuaternion();
    double velocity = 0;

    double stanleyGainK = 1;
    double stanleyGainKSoft = 1;
    double stanleyGainDampeningYaw = 1;
    double stanleyGainDampeningSteering = 1;
    double maxSteeringAngle = 40;

    double steeringAngle = 0;
    double steeringAngle1Ago = 0;
    double steeringAngle2Ago = 0;

    double yawTrajectory1Ago = 0;

  private:
    QVector<QSharedPointer<PathPrimitive>> plan;
};

class StanleyGuidanceFactory : public BlockFactory {
    Q_OBJECT

  public:
    StanleyGuidanceFactory( Tile* tile )
      : BlockFactory(),
        tile( tile ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Stanley Path Follower" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new StanleyGuidance( tile );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj, true );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Pose", SLOT( setPose( Tile*, QVector3D, QQuaternion ) ) );
      b->addInputPort( "Steering Angle", SLOT( setSteeringAngle( float ) ) );
      b->addInputPort( "Velocity", SLOT( setVelocity( float ) ) );
      b->addInputPort( "Plan", SLOT( setPlan( QVector<QSharedPointer<PathPrimitive>> ) ) );
      b->addInputPort( "Stanley Gain K", SLOT( setStanleyGainK( float ) ) );
      b->addInputPort( "Stanley Gain K Softening", SLOT( setStanleyGainKSoft( float ) ) );
      b->addInputPort( "Stanley Gain Yaw Dampening", SLOT( setStanleyGainDampeningYaw( float ) ) );
      b->addInputPort( "Stanley Gain Steering Dampening", SLOT( setStanleyGainDampeningSteering( float ) ) );
      b->addInputPort( "Max Steering Angle", SLOT( setMaxSteeringAngle( float ) ) );

      b->addOutputPort( "XTE", SIGNAL( xteChanged( float ) ) );
      b->addOutputPort( "Steer Angle", SIGNAL( steerAngleChanged( float ) ) );

      return b;
    }

  private:
    Tile* tile;
};

#endif // STANLEYGUIDANCE_H

