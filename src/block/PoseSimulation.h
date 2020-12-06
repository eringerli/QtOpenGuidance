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

#include "../helpers/cgalHelper.h"
#include "../helpers/eigenHelper.h"

#include "../helpers/GeographicConvertionWrapper.h"
#include "../kinematic/FixedKinematicPrimitive.h"

#include "../filter/3wFRHRL/SystemModel.h"

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
      auto offsetTmp = offset;
      offsetTmp.x() -= b;
      antennaKinematic.setOffset( -offsetTmp );
    }

    void setInitialWGS84Position( const Eigen::Vector3d position ) {
      geographicConvertionWrapper->Reset( position.x(), position.y(), position.z() );
    }

    void autosteerEnabled( bool enabled ) {
      m_autosteerEnabled = enabled;
    }

    void setSimulatorValues( const double a, const double b, const double c,
                             const double Caf, const double Car, const double Cah,
                             const double m, const double Iz,
                             const double sigmaF, const double sigmaR, const double sigmaH,
                             const double Cx, const double slipX );

    void setNoiseStandartDeviations( double noisePositionXY,
                                     double noisePositionZ,
                                     double noiseOrientation,
                                     double noiseAccelerometer,
                                     double noiseGyro );

  protected:
    void timerEvent( QTimerEvent* event ) override;

  signals:
    void simulatorValuesChanged( const double a, const double b, const double c,
                                 const double Caf, const double Car, const double Cah,
                                 const double m, const double Iz,
                                 const double sigmaF, const double sigmaR, const double sigmaH,
                                 const double Cx, const double slip );

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
      qDebug() << "PoseSimulation::emitConfigSignals";
      emit simulatorValuesChanged( a, b, c,
                                   // N/rad -> N/°
                                   Caf * M_PI / 180, Car * M_PI / 180, Cah * M_PI / 180,
                                   m, Iz,
                                   sigmaF, sigmaR, sigmaH,
                                   Cx, slipX );

      emit steerAngleChanged( m_steerAngle );
      emit positionChanged( Eigen::Vector3d( x, y, height ) );
      emit orientationChanged( m_orientation );
      emit steerAngleChanged( 0 );
      emit velocityChanged( 0 );
    }

    virtual void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;

      valuesObject[QStringLiteral( "a" )] = this->a;
      valuesObject[QStringLiteral( "b" )] = this->b;
      valuesObject[QStringLiteral( "c" )] = this->c;
      valuesObject[QStringLiteral( "Caf" )] = this->Caf;
      valuesObject[QStringLiteral( "Car" )] = this->Car;
      valuesObject[QStringLiteral( "Cah" )] = this->Cah;
      valuesObject[QStringLiteral( "m" )] = this->m;
      valuesObject[QStringLiteral( "Iz" )] = this->Iz;
      valuesObject[QStringLiteral( "sigmaF" )] = this->sigmaF;
      valuesObject[QStringLiteral( "sigmaR" )] = this->sigmaR;
      valuesObject[QStringLiteral( "sigmaH" )] = this->sigmaH;
      valuesObject[QStringLiteral( "Cx" )] = this->Cx;
      valuesObject[QStringLiteral( "slip" )] = this->slipX;

      json[QStringLiteral( "values" )] = valuesObject;
    }

    virtual void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        if( valuesObject[QStringLiteral( "a" )].isDouble() ) {
          this->a = valuesObject[QStringLiteral( "a" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "b" )].isDouble() ) {
          this->b = valuesObject[QStringLiteral( "b" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "c" )].isDouble() ) {
          this->c = valuesObject[QStringLiteral( "c" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Caf" )].isDouble() ) {
          this->Caf = valuesObject[QStringLiteral( "Caf" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Car" )].isDouble() ) {
          this->Car = valuesObject[QStringLiteral( "Car" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Cah" )].isDouble() ) {
          this->Cah = valuesObject[QStringLiteral( "Cah" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "m" )].isDouble() ) {
          this->m = valuesObject[QStringLiteral( "m" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Iz" )].isDouble() ) {
          this->Iz = valuesObject[QStringLiteral( "Iz" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "sigmaF" )].isDouble() ) {
          this->sigmaF = valuesObject[QStringLiteral( "sigmaF" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "sigmaR" )].isDouble() ) {
          this->sigmaR = valuesObject[QStringLiteral( "sigmaR" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "sigmaH" )].isDouble() ) {
          this->sigmaH = valuesObject[QStringLiteral( "sigmaH" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "Cx" )].isDouble() ) {
          this->Cx = valuesObject[QStringLiteral( "Cx" )].toDouble();
        }

        if( valuesObject[QStringLiteral( "slip" )].isDouble() ) {
          this->slipX = valuesObject[QStringLiteral( "slip" )].toDouble();
        }
      }
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

    double a = 1;
    double b = 2;
    double c = 2.5;
    double Caf = 2400 * 180 / M_PI;
    double Car = 5000 * 180 / M_PI;
    double Cah = 750 * 180 / M_PI;
    double m = 5500;
    double Iz = 9500;
    double sigmaF = 0.34;
    double sigmaR = 0.34;
    double sigmaH = 0.40;
    double Cx = 5000 * 180 / M_PI;
    double slipX = 0.025;

    double m_wheelbase = 2.4f;

    Eigen::Quaterniond m_orientation = Eigen::Quaterniond();

    double x = 0;
    double y = 0;
    double height = 0;
    double lastHeading = 0;
    GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
    FixedKinematicPrimitive antennaKinematic;


    template <typename T_> using StateType = ThreeWheeledFRHRL::State<T_>;
    StateType<double> state;
    ThreeWheeledFRHRL::TractorImuGpsFusionModel<StateType> simulatorModel;

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
