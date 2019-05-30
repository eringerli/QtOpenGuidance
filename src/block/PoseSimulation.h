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

#ifndef POSESIMULATION_H
#define POSESIMULATION_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "GuidanceBase.h"

class PoseSimulation : public GuidanceBase {
    Q_OBJECT

  public:
    explicit PoseSimulation()
      : GuidanceBase(),
        m_enabled( false ), m_interval( 50 ), m_timerId( 0 ), m_steerAngle( 0 ), m_velocity( 0 ), m_wheelbase( 2.4F ), m_position(), m_orientation() {
      setSimulation( false );
      m_system = true;
      m_id = getNextSystemId();
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
      emit steerAngleChanged( m_steerAngle );
    }

    void setVelocity( float velocity ) {
      m_velocity = velocity;
      emit velocityChanged( velocity );
    }

    void setWheelbase( float wheelbase ) {
      m_wheelbase = wheelbase;
      emit wheelbaseChanged( m_wheelbase );
    }

    void setAntennaPosition( QVector3D position ) {
      m_antennaPosition = position;
      emit antennaPositionChanged( m_position );
    }

    void setPosition( QVector3D position ) {
      m_position = position;
      emit positionChanged( m_position );
    }

    void setOrientation( QQuaternion orientation ) {
      m_orientation = orientation;
      emit orientationChanged( m_orientation );
    }


  protected:
    void timerEvent( QTimerEvent* event ) override;

  signals:
    void simulationChanged( bool enabled );
    void intervalChanged( int interval );

    void steerAngleChanged( float steerAngle );
    void velocityChanged( float velocity );
    void wheelbaseChanged( float wheelbase );

    void antennaPositionChanged( QVector3D position );

    void steeringAngleChanged( float );
    void positionChanged( QVector3D position );
    void positionChangedRelative( QVector3D position );
    void orientationChanged( QQuaternion orientation );
    void orientationChangedRelative( QQuaternion orientation );

  public:
    virtual void emitConfigSignals() override {
      emit steerAngleChanged( m_steerAngle );
      emit positionChanged( m_position );
      emit orientationChanged( m_orientation );
    }

  private:
    bool m_enabled;
    int m_interval;

    QBasicTimer m_timer;
    int m_timerId;
    QTime m_time;

    float m_steerAngle;
    float m_velocity;
    float m_wheelbase;

    QVector3D m_antennaPosition;
    QVector3D m_position;
    QQuaternion m_orientation;
};

class PoseSimulationFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    PoseSimulationFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Pose Simulation" );
    }

    virtual void addToCombobox( QComboBox* ) override {
    }

    virtual GuidanceBase* createNewObject() override {
      return new PoseSimulation;
    }

    virtual void createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->systemBlock = true;

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "Antenna Position", SLOT( setAntennaPosition( QVector3D ) ) );

      b->addOutputPort( "Position", SIGNAL( positionChanged( QVector3D ) ) );
      b->addOutputPort( "Orientation", SIGNAL( orientationChanged( QQuaternion ) ) );
      b->addOutputPort( "Steering Angle", SIGNAL( steeringAngleChanged( float ) ) );
    }
};

#endif // POSESIMULATION_H

