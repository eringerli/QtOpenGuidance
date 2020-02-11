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

#ifndef POSESIMULATION_H
#define POSESIMULATION_H

#include <QObject>

#include <QEvent>
#include <QBasicTimer>
#include <QElapsedTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "BlockBase.h"

#include "../cgalKernel.h"

#include "../kinematic/GeographicConvertionWrapper.h"

using namespace std;
using namespace GeographicLib;

class PoseSimulation : public BlockBase {
    Q_OBJECT

  public:
    explicit PoseSimulation( GeographicConvertionWrapper* geographicConvertionWrapper )
      : BlockBase(),
        geographicConvertionWrapper( geographicConvertionWrapper ) {
      setSimulation( false );
    }

  public slots:
    void setInterval( int interval ) {
      m_interval = interval;

      setSimulation( m_enabled );

      emit intervalChanged( m_interval );
    }

    void setFrequency( int frequency ) {
      setInterval( 1000 / frequency );
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

    void setSteerAngle( float steerAngle ) {
      m_steerAngle = steerAngle;
    }

    void setSteerAngleFromAutosteer( float steerAngle ) {
      m_steerAngleFromAutosteer = steerAngle;
    }

    void setVelocity( float velocity ) {
      m_velocity = velocity;
    }

    void setWheelbase( float wheelbase ) {
      if( !qFuzzyIsNull( wheelbase ) ) {
        m_wheelbase = wheelbase;
      }
    }

    void setAntennaPosition( QVector3D position ) {
      m_antennaPosition = position;
    }

    void setInitialWGS84Position( QVector3D position ) {
      geographicConvertionWrapper->Reset( position.x(), position.y(), position.z() );
    }

    void autosteerEnabled( bool enabled ) {
      m_autosteerEnabled = enabled;
    }

  protected:
    void timerEvent( QTimerEvent* event ) override;

  signals:
    void simulationChanged( bool );
    void intervalChanged( int );

    void steerAngleChanged( float );

    void antennaPositionChanged( QVector3D );

    void steeringAngleChanged( float );
    void positionChanged( QVector3D );
    void globalPositionChanged( double, double, double );
    void orientationChanged( QQuaternion );
    void velocityChanged( float );

  public:
    virtual void emitConfigSignals() override {
      emit steerAngleChanged( m_steerAngle );
      emit positionChanged( QVector3D( float( x ), float( y ), float( height ) ) );
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

    float m_steerAngle = 0;
    float m_steerAngleFromAutosteer = 0;
    float m_velocity = 0;
    float m_wheelbase = 2.4f;

    QVector3D m_antennaPosition = QVector3D();
    QQuaternion m_orientation = QQuaternion();

    double x = 0;
    double y = 0;
    double height = 0;
    GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
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

    virtual void addToCombobox( QComboBox* ) override {}

    virtual QNEBlock* createBlock( QGraphicsScene* scene ) override {
      auto* obj = new PoseSimulation( geographicConvertionWrapper );
      auto* b = createBaseBlock( scene, obj, true );

      b->addInputPort( QStringLiteral( "Antenna Position" ), QLatin1String( SLOT( setAntennaPosition( QVector3D ) ) ) );
      b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( float ) ) ) );
      b->addInputPort( QStringLiteral( "Initial WGS84 Position" ), QLatin1String( SLOT( setInitialWGS84Position( QVector3D ) ) ) );

      b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( double, double, double ) ) ) );
      b->addOutputPort( QStringLiteral( "Position" ), QLatin1String( SIGNAL( positionChanged( QVector3D ) ) ) );
      b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( QQuaternion ) ) ) );
      b->addOutputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SIGNAL( steeringAngleChanged( float ) ) ) );
      b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( float ) ) ) );

      b->addInputPort( QStringLiteral( "Autosteer Enabled" ), QLatin1String( SLOT( autosteerEnabled( bool ) ) ) );
      b->addInputPort( QStringLiteral( "Autosteer Steering Angle" ), QLatin1String( SLOT( setSteerAngleFromAutosteer( float ) ) ) );

      return b;
    }

  private:
    GeographicConvertionWrapper* geographicConvertionWrapper = nullptr;
};

#endif // POSESIMULATION_H

