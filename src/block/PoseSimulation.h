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

#include <random>
#include <chrono>

#include <QObject>

#include <QEvent>
#include <QBasicTimer>
#include <QElapsedTimer>

#include <QtGlobal>
#include <QtDebug>

#include "BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"

#include "../kinematic/GeographicConvertionWrapper.h"
#include "../kinematic/FixedKinematicPrimitive.h"

#include "../filter/BicycleModel/SystemModel.h"

using namespace std;
using namespace GeographicLib;

class PoseSimulation : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseSimulation( GeographicConvertionWrapper* geographicConvertionWrapper );

  public slots:
    void setInterval( int interval ) {
      m_interval = interval;

      setSimulation( m_enabled );

      emit intervalChanged( m_interval );
    }

    void setSimulation( bool enabled ) {
      m_enabled = enabled;

      if( enabled ) {
        m_timer.start( m_interval, Qt::PreciseTimer, this );
        m_time.start();
      } else {
        m_timer.stop();
      }

      emit simulationChanged( m_enabled );
    }


    void setFrequency( double frequency ) {
      setInterval( 1000 / frequency );
    }

    void setSteerAngle( double steerAngle ) {
      m_steerAngle = steerAngle + m_steerAngleOffset;
    }

    void setVelocity( double velocity ) {
      m_velocity = velocity;
    }

    void setRollOffset( double offset ) {
      m_rollOffset = offset;
    }

    void setPitchOffset( double offset ) {
      m_pitchOffset = offset;
    }

    void setYawOffset( double offset ) {
      m_yawOffset = offset;
    }

    void setSteerAngleOffset( double offset ) {
      m_steerAngleOffset = offset;
    }

    void setSteerAngleFromAutosteer( double steerAngle ) {
      m_steerAngleFromAutosteer = steerAngle + m_steerAngleOffset;
    }


    void setWheelbase( double wheelbase ) {
      if( !qFuzzyIsNull( wheelbase ) ) {
        m_wheelbase = wheelbase;
      }
    }

    void setAntennaOffset( const Eigen::Vector3d offset ) {
      antennaKinematic.setOffset( -offset );
    }

    void setInitialWGS84Position( const Eigen::Vector3d position ) {
      geographicConvertionWrapper->Reset( position.x(), position.y(), position.z() );
    }

    void autosteerEnabled( bool enabled ) {
      m_autosteerEnabled = enabled;
    }

    void setNoiseStandartDeviations( double noisePositionXY,
                                     double noisePositionZ,
                                     double noiseOrientation,
                                     double noiseAccelerometer,
                                     double noiseGyro );

  protected:
    void timerEvent( QTimerEvent* event ) override;

  signals:
    void simulationChanged( bool );
    void intervalChanged( int );

    void steerAngleChanged( double );

    void antennaPositionChanged( const Eigen::Vector3d );

    void steeringAngleChanged( double );
    void positionChanged( const Eigen::Vector3d );
    void globalPositionChanged( const Eigen::Vector3d );
    void velocity3DChanged( const Eigen::Vector3d );
    void orientationChanged( const Eigen::Quaterniond );
    void velocityChanged( double );

    void imuDataChanged( const Eigen::Quaterniond, const Eigen::Vector3d, const Eigen::Vector3d );

  public:
    virtual void emitConfigSignals() override {
      emit steerAngleChanged( m_steerAngle );
      emit positionChanged( Eigen::Vector3d( x, y, height ) );
      emit orientationChanged( m_orientation );
      emit steerAngleChanged( 0 );
      emit velocityChanged( 0 );
    }

  private:
    bool m_enabled = false;
    bool m_autosteerEnabled = false;
    int m_interval = 50;

    QBasicTimer m_timer;
    int m_timerId;
    QElapsedTimer m_time;

    double m_steerAngle = 0;
    double m_steerAngleFromAutosteer = 0;
    double m_velocity = 0;
    double m_rollOffset = 0;
    double m_pitchOffset = 0;
    double m_yawOffset = 0;
    double m_steerAngleOffset = 0;

    double m_wheelbase = 2.4f;

    Eigen::Quaterniond m_orientation = Eigen::Quaterniond();

    double x = 0;
    double y = 0;
    double height = 0;
    double lastHeading = 0;
    GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
    FixedKinematicPrimitive antennaKinematic;

    template <typename T_> using StateType = KinematicModel::State<T_>;
    StateType<double> state;

    KinematicModel::TractorImuGpsFusionModel<StateType> simulatorModel;

    std::default_random_engine noiseGenerator;
    bool noisePositionXYActivated = false;
    std::normal_distribution<double> noisePositionXY = std::normal_distribution<double>( 0, 0.02 );
    bool noisePositionZActivated = false;
    std::normal_distribution<double> noisePositionZ = std::normal_distribution<double>( 0, 0.04 );
    bool noiseOrientationActivated = false;
    std::normal_distribution<double> noiseOrientation = std::normal_distribution<double>( 0, 0.001 );
    bool noiseAccelerometerActivated = false;
    std::normal_distribution<double> noiseAccelerometer = std::normal_distribution<double>( 0, 0.01 );
    bool noiseGyroActivated = false;
    std::normal_distribution<double> noiseGyro = std::normal_distribution<double>( 0, 0.01 );
};

class PoseSimulationFactory : public BlockFactory {
    Q_OBJECT

  public:
    PoseSimulationFactory( GeographicConvertionWrapper* geographicConvertionWrapper )
      : BlockFactory(),
        geographicConvertionWrapper( geographicConvertionWrapper ) {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose Simulation" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Base Blocks" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new PoseSimulation( geographicConvertionWrapper );
      auto* b = createBaseBlock( scene, obj, id, true );

      b->addInputPort( QStringLiteral( "Antenna Position" ), QLatin1String( SLOT( setAntennaOffset( const Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( double ) ) ) );
      b->addInputPort( QStringLiteral( "Initial WGS84 Position" ), QLatin1String( SLOT( setInitialWGS84Position( const Eigen::Vector3d ) ) ) );

      b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const Eigen::Vector3d ) ) ) );
      b->addOutputPort( QStringLiteral( "Velocity 3D" ), QLatin1String( SIGNAL( velocity3DChanged( const Eigen::Vector3d ) ) ) );

      b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( const Eigen::Vector3d ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( const Eigen::Quaterniond ) ) ) );
      b->addOutputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SIGNAL( steeringAngleChanged( double ) ) ) );
      b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( double ) ) ) );

      b->addOutputPort( QStringLiteral( "IMU Data" ), QLatin1String( SIGNAL( imuDataChanged( const Eigen::Quaterniond, const Eigen::Vector3d, const Eigen::Vector3d ) ) ) );

      b->addInputPort( QStringLiteral( "Autosteer Enabled" ), QLatin1String( SLOT( autosteerEnabled( bool ) ) ) );
      b->addInputPort( QStringLiteral( "Autosteer Steering Angle" ), QLatin1String( SLOT( setSteerAngleFromAutosteer( double ) ) ) );

      return b;
    }

  private:
    GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
};
