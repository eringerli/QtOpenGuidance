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

#ifndef TRAILERKINEMATIC_H
#define TRAILERKINEMATIC_H

#include <QtCore/QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtMath>
#include <QtDebug>

#include "../block/GuidanceBase.h"

class TrailerKinematic : public GuidanceBase {
    Q_OBJECT

  public:
    explicit TrailerKinematic()
      : GuidanceBase(),
        m_offsetHookPoint( QVector3D( 6, 0, 0 ) ), m_offsetTowPoint( QVector3D( -1, 0, 0 ) ), m_positionPivotPoint( QVector3D( 0, 0, 0 ) ) {
    }

  public slots:
    void setOffsetTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( QVector3D position ) {
      m_offsetHookPoint = position;
    }

    void setPose( QVector3D position, QQuaternion orientation ) {
      QQuaternion orientationTrailer = QQuaternion::fromAxisAndAngle(
                                         QVector3D( 0.0f, 0.0f, 1.0f ),
                                         qRadiansToDegrees( qAtan2( position.y() - m_positionPivotPoint.y(), position.x() - m_positionPivotPoint.x() ) )
                                       );

      // the angle between tractor and trailer >120° -> reset orientation to the one from the tractor
      if( qAbs( ( orientation.inverted()*orientationTrailer ).toEulerAngles().z() ) < 120 ) {
        orientation = orientationTrailer;
      }

      m_positionPivotPoint = position + orientation * -m_offsetHookPoint;
      QVector3D positionTowPoint = m_positionPivotPoint + orientation * m_offsetTowPoint;

      emit poseHookPointChanged( position, orientation );
      emit posePivotPointChanged( m_positionPivotPoint, orientation );
      emit poseTowPointChanged( positionTowPoint, orientation );
    }

  signals:
    void poseHookPointChanged( QVector3D, QQuaternion );
    void posePivotPointChanged( QVector3D, QQuaternion );
    void poseTowPointChanged( QVector3D, QQuaternion );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint;
    QVector3D m_offsetTowPoint;

    QVector3D m_positionPivotPoint;
};

class TrailerKinematicFactory : public GuidanceFactory {
    Q_OBJECT

  public:
    TrailerKinematicFactory()
      : GuidanceFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Kinematic" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual GuidanceBase* createNewObject() override {
      return new TrailerKinematic;
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      QNEBlock* b = new QNEBlock( obj );
      scene->addItem( b );

      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::NamePort );
      b->addPort( getNameOfFactory(), QStringLiteral( "" ), 0, QNEPort::TypePort );

      b->addInputPort( "OffsetHookPoint", SLOT( setOffsetHookPointPosition( QVector3D ) ) );
      b->addInputPort( "OffsetTowPoint", SLOT( setOffsetTowPointPosition( QVector3D ) ) );
      b->addInputPort( "Pose", SLOT( setPose( QVector3D, QQuaternion ) ) );

      b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( QVector3D, QQuaternion ) ) );
      b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( QVector3D, QQuaternion ) ) );

      return b;
    }
};

#endif // TRAILERKINEMATIC_H
