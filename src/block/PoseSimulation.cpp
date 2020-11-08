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
  setSimulation( false );

  state.setZero();

  noiseGenerator.seed( std::chrono::system_clock::now().time_since_epoch().count() );
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
    double elapsedTime = double ( m_time.restart() ) / msPerS;
    double steerAngle = 0;

    if( m_autosteerEnabled ) {
      steerAngle = m_steerAngleFromAutosteer;
    } else {
      steerAngle = m_steerAngle;
    }

    emit steeringAngleChanged( steerAngle );
    emit velocityChanged( m_velocity );

    {
      StateType<double> prediction;
      simulatorModel.predict( state, elapsedTime, m_velocity, steerAngle, m_wheelbase, prediction );
      state = prediction;
    }

    qDebug() << "State: pos x|y|z" << state( int( KinematicModel::StateNames::X ) ) << state( int( KinematicModel::StateNames::Y ) ) << state( int( KinematicModel::StateNames::Z ) );
    qDebug() << "State: vel x|y|z" << state( int( KinematicModel::StateNames::Vx ) ) << state( int( KinematicModel::StateNames::Vy ) ) << state( int( KinematicModel::StateNames::Vz ) );
    qDebug() << "State: acc x|y|z" << state( int( KinematicModel::StateNames::Ax ) ) << state( int( KinematicModel::StateNames::Ay ) ) << state( int( KinematicModel::StateNames::Az ) );
    qDebug() << "State: vel r|p|y" << state( int( KinematicModel::StateNames::Vroll ) ) << state( int( KinematicModel::StateNames::Vpitch ) ) << state( int( KinematicModel::StateNames::Vyaw ) );
    qDebug() << "State: ang r|p|y" << state( int( KinematicModel::StateNames::Roll ) ) << state( int( KinematicModel::StateNames::Pitch ) ) << state( int( KinematicModel::StateNames::Yaw ) );
    qDebug() << "";

    // orientation
    {

      if( noiseOrientationActivated ) {
        m_orientation = eulerToQuaternion( state( int( KinematicModel::StateNames::Roll ) ) + noiseOrientation( noiseGenerator ),
                                           state( int( KinematicModel::StateNames::Pitch ) ) + noiseOrientation( noiseGenerator ),
                                           state( int( KinematicModel::StateNames::Yaw ) ) + noiseOrientation( noiseGenerator ) );
      } else {
        m_orientation = eulerToQuaternion( state( int( KinematicModel::StateNames::Roll ) ),
                                           state( int( KinematicModel::StateNames::Pitch ) ),
                                           state( int( KinematicModel::StateNames::Yaw ) ) );

      }

      emit orientationChanged( m_orientation );
    }

    {
      /*static uint8_t counter = 0;

      if( ++counter >= 10 )*/ {
//        counter = 0;

        if( noisePositionXYActivated || noisePositionZActivated ) {
          antennaKinematic.setPose( Point_3( state( int( KinematicModel::StateNames::X ) ) + noisePositionXY( noiseGenerator ),
                                             state( int( KinematicModel::StateNames::Y ) ) + noisePositionXY( noiseGenerator ),
                                             state( int( KinematicModel::StateNames::Z ) ) + noisePositionZ( noiseGenerator ) ),
                                    m_orientation,
                                    PoseOption::Options() );
        } else {
          antennaKinematic.setPose( Point_3( state( int( KinematicModel::StateNames::X ) ),
                                             state( int( KinematicModel::StateNames::Y ) ),
                                             state( int( KinematicModel::StateNames::Z ) ) ),
                                    m_orientation,
                                    PoseOption::Options() );
        }

        emit velocity3DChanged( Eigen::Vector3d( state( int( KinematicModel::StateNames::Vx ) ),
                                state( int( KinematicModel::StateNames::Vy ) ),
                                state( int( KinematicModel::StateNames::Vz ) ) ) );

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
      accelerometerData = Eigen::Vector3d( state( int( KinematicModel::StateNames::Ax ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( KinematicModel::StateNames::Ay ) ) + noiseAccelerometer( noiseGenerator ),
                                           state( int( KinematicModel::StateNames::Az ) ) + noiseAccelerometer( noiseGenerator ) );
    } else {
      accelerometerData = Eigen::Vector3d( state( int( KinematicModel::StateNames::Ax ) ),
                                           state( int( KinematicModel::StateNames::Ay ) ),
                                           state( int( KinematicModel::StateNames::Az ) ) );
    }

    Eigen::Vector3d gyroData;

    if( noiseGyroActivated ) {
      gyroData = Eigen::Vector3d( state( int( KinematicModel::StateNames::Vroll ) ) + noiseGyro( noiseGenerator ),
                                  state( int( KinematicModel::StateNames::Vpitch ) ) + noiseGyro( noiseGenerator ),
                                  state( int( KinematicModel::StateNames::Vyaw ) ) + noiseGyro( noiseGenerator ) );
    } else {
      gyroData = Eigen::Vector3d( state( int( KinematicModel::StateNames::Vroll ) ),
                                  state( int( KinematicModel::StateNames::Vpitch ) ),
                                  state( int( KinematicModel::StateNames::Vyaw ) ) );
    }

    emit imuDataChanged( m_orientation, accelerometerData, gyroData );
//      qDebug() << "Cycle Time PoseSimulation:    " << timer.nsecsElapsed() << "ns";
  }
}
