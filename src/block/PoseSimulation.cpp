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

#include "PoseSimulation.h"

#include <QTime>
#include <QEvent>
#include <QtMath>

#include <QDebug>

#include "../kinematic/eigenHelper.h"

// http://correll.cs.colorado.edu/?p=1869
// https://github.com/correll/Introduction-to-Autonomous-Robots/releases

PoseSimulation::PoseSimulation( GeographicConvertionWrapper* geographicConvertionWrapper )
  : BlockBase(),
    geographicConvertionWrapper( geographicConvertionWrapper ) {

  state.setZero();

  setSimulation( false );

  noiseGenerator.seed( std::chrono::system_clock::now().time_since_epoch().count() );
}

void PoseSimulation::setSimulatorValues( const double a, const double b, const double c, const double Caf, const double Car, const double Cah, const double m, const double Iz, const double sigmaF, const double sigmaR, const double sigmaH, const double Cx, const double slipX ) {
  this->a = a;
  this->b = b;
  this->c = c;
  // N/° -> N/rad
  this->Caf = Caf * 180 / M_PI;
  this->Car = Car * 180 / M_PI;
  this->Cah = Cah * 180 / M_PI;
  this->m = m;
  this->Iz = Iz;
  this->sigmaF = sigmaF;
  this->sigmaR = sigmaR;
  this->sigmaH = sigmaH;
  this->Cx = Cx;
  this->slipX = slipX;
}

void PoseSimulation::setNoiseStandartDeviations( double noisePositionXY, double noisePositionZ, double noiseOrientation, double noiseAccelerometer, double noiseGyro ) {
  if( !qIsNull( noisePositionXY ) ) {
    noisePositionXYActivated = true;
    this->noisePositionXY = std::normal_distribution<double>( 0, noisePositionXY );
  } else {
    noisePositionXYActivated = false;
  }

  if( !qIsNull( noisePositionZ ) ) {
    noisePositionZActivated = true;
    this->noisePositionZ = std::normal_distribution<double>( 0, noisePositionZ );
  } else {
    noisePositionZActivated = false;
  }

  if( !qIsNull( noiseOrientation ) ) {
    noiseOrientationActivated = true;
    this->noiseOrientation = std::normal_distribution<double>( 0, noiseOrientation );
  } else {
    noiseOrientationActivated = false;
  }

  if( !qIsNull( noiseAccelerometer ) ) {
    noiseAccelerometerActivated = true;
    this->noiseAccelerometer = std::normal_distribution<double>( 0, noiseAccelerometer );
  } else {
    noiseAccelerometerActivated = false;
  }

  if( !qIsNull( noiseGyro ) ) {
    noiseGyroActivated = true;
    this->noiseGyro = std::normal_distribution<double>( 0, noiseGyro );
  } else {
    noiseGyroActivated = false;
  }
}

void PoseSimulation::timerEvent( QTimerEvent* event ) {
  if( event->timerId() == m_timer.timerId() ) {
    constexpr double msPerS = 1000;
    double deltaT = double ( m_time.restart() ) / msPerS;
    double steerAngle = 0;

    if( m_autosteerEnabled ) {
      steerAngle = m_steerAngleFromAutosteer;
    } else {
      steerAngle = m_steerAngle;
    }

    emit steeringAngleChanged( steerAngle );
    emit velocityChanged( m_velocity );

    {
      auto& V = m_velocity;
      auto deltaF = qDegreesToRadians( steerAngle );

      {
        constexpr double MinDeltaT = 0.001;
        double factor = std::floor( deltaT / MinDeltaT );
        double deltaT2 = deltaT / factor;

        std::cout << "factor, deltaT, deltaT2" << factor << deltaT << deltaT2 << std::endl;

        for( int i = 0; i < factor; ++i ) {
          StateType<double> prediction;
          qDebug() << "PoseSimulation::timerEvent" << Caf << Car << Cah;
          simulatorModel.predict( state, deltaT2,
                                  V, deltaF,
                                  a, b, c,
                                  Caf, Car, Cah,
                                  m, Iz,
                                  sigmaF, sigmaR, sigmaH,
                                  Cx, slipX,
                                  prediction );
          state = prediction;
        }
      }
    }

    // orientation
    {

      if( noiseOrientationActivated ) {
        m_orientation = eulerToQuaternion( state( int( ThreeWheeledFRHRL::StateNames::Roll ) ) + noiseOrientation( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ) + noiseOrientation( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) + noiseOrientation( noiseGenerator ) );
      } else {
        m_orientation = eulerToQuaternion( state( int( ThreeWheeledFRHRL::StateNames::Roll ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Pitch ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Yaw ) ) );

      }

      emit orientationChanged( m_orientation );
    }

    {
      /*static uint8_t counter = 0;

      if( ++counter >= 10 )*/ {
//        counter = 0;

        if( noisePositionXYActivated || noisePositionZActivated ) {
          antennaKinematic.setPose( Point_3( state( int( ThreeWheeledFRHRL::StateNames::X ) ) + noisePositionXY( noiseGenerator ),
                                             state( int( ThreeWheeledFRHRL::StateNames::Y ) ) + noisePositionXY( noiseGenerator ),
                                             state( int( ThreeWheeledFRHRL::StateNames::Z ) ) + noisePositionZ( noiseGenerator ) ),
                                    m_orientation,
                                    PoseOption::Options() );
        } else {
          antennaKinematic.setPose( Point_3( state( int( ThreeWheeledFRHRL::StateNames::X ) ),
                                             state( int( ThreeWheeledFRHRL::StateNames::Y ) ),
                                             state( int( ThreeWheeledFRHRL::StateNames::Z ) ) ),
                                    m_orientation,
                                    PoseOption::Options() );
        }

        emit velocity3DChanged( Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vx ) ),
                                state( int( ThreeWheeledFRHRL::StateNames::Vy ) ),
                                state( int( ThreeWheeledFRHRL::StateNames::Vz ) ) ) );

        // emit signal with antenna offset
        emit positionChanged( toEigenVector( antennaKinematic.positionCalculated ) );


        // in global coordinates: WGS84
        {
          double latitude, longitude, height;
          geographicConvertionWrapper->Reverse( antennaKinematic.positionCalculated.x(),
                                                antennaKinematic.positionCalculated.y(),
                                                antennaKinematic.positionCalculated.z(), latitude, longitude, height );

//      QElapsedTimer timer;
//      timer.start();
          emit globalPositionChanged( Eigen::Vector3d( latitude, longitude, height ) );

        }
      }
    }


    Eigen::Vector3d accelerometerData;

    if( noiseAccelerometerActivated ) {
      accelerometerData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Ax ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Ay ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Az ) ) + noiseAccelerometer( noiseGenerator ) );
    } else {
      accelerometerData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Ax ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Ay ) ),
                                           state( int( ThreeWheeledFRHRL::StateNames::Az ) ) );
    }

    Eigen::Vector3d gyroData;

    if( noiseGyroActivated ) {
      gyroData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vroll ) ) + noiseGyro( noiseGenerator ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vpitch ) ) + noiseGyro( noiseGenerator ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vyaw ) ) + noiseGyro( noiseGenerator ) );
    } else {
      gyroData = Eigen::Vector3d( state( int( ThreeWheeledFRHRL::StateNames::Vroll ) ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vpitch ) ),
                                  state( int( ThreeWheeledFRHRL::StateNames::Vyaw ) ) );
    }

    emit imuDataChanged( m_orientation, accelerometerData, gyroData );
//      qDebug() << "Cycle Time PoseSimulation:    " << timer.nsecsElapsed() << "ns";
  }
}
