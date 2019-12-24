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

#ifndef TRACTORKINEMATIC_H
#define TRACTORKINEMATIC_H

#include <QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QQuaternion>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "../block/BlockBase.h"

#include "../cgalKernel.h"
#include "../kinematic/PoseOptions.h"

class FixedKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : BlockBase(),
        m_offsetHookPoint( QVector3D( 0, 0, 0 ) ), m_offsetTowPoint( QVector3D( -1, 0, 0 ) ) {}

  public slots:
    void setOffsetTowPointPosition( QVector3D position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( QVector3D position ) {
      m_offsetHookPoint = position;
    }

    void setPose( Point_3 position, QQuaternion orientation, PoseOption::Options options ) {
      if( options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation = QQuaternion();
      }

      QVector3D positionPivotPointCorrection;

      if( !options.testFlag( PoseOption::CalculateFromPivotPoint ) ) {
        positionPivotPointCorrection = orientation * -m_offsetHookPoint;
      }

      Point_3 positionPivotPoint = Point_3( position.x() + double( positionPivotPointCorrection.x() ),
                                            position.y() + double( positionPivotPointCorrection.y() ),
                                            position.z() + double( positionPivotPointCorrection.z() ) );

      QVector3D positionTowPointCorrection = orientation * m_offsetTowPoint;
      Point_3 positionTowPoint = Point_3( positionPivotPoint.x() + double( positionTowPointCorrection.x() ),
                                          positionPivotPoint.y() + double( positionTowPointCorrection.y() ),
                                          positionPivotPoint.z() + double( positionTowPointCorrection.z() ) );

      options.setFlag( PoseOption::CalculateFromPivotPoint, false );

      emit poseHookPointChanged( position, orientation, options );
      emit posePivotPointChanged( positionPivotPoint, orientation, options );
      emit poseTowPointChanged( positionTowPoint, orientation, options );
    }

  signals:
    void poseHookPointChanged( Point_3, QQuaternion, PoseOption::Options );
    void posePivotPointChanged( Point_3, QQuaternion, PoseOption::Options );
    void poseTowPointChanged( Point_3, QQuaternion, PoseOption::Options );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    QVector3D m_offsetHookPoint;
    QVector3D m_offsetTowPoint;
};

class FixedKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic" );
    }

    virtual void addToCombobox( QComboBox* combobox ) override {
      combobox->addItem( getNameOfFactory(), QVariant::fromValue( this ) );
    }

    virtual BlockBase* createNewObject() override {
      return new FixedKinematic;
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, QObject* obj ) override {
      auto* b = createBaseBlock( scene, obj );

      b->addInputPort( "OffsetHookPoint", SLOT( setOffsetHookPointPosition( QVector3D ) ) );
      b->addInputPort( "OffsetTowPoint", SLOT( setOffsetTowPointPosition( QVector3D ) ) );
      b->addInputPort( "Pose", SLOT( setPose( Point_3, QQuaternion, PoseOption::Options ) ) );

      b->addOutputPort( "Pose Hook Point", SIGNAL( poseHookPointChanged( Point_3, QQuaternion, PoseOption::Options ) ) );
      b->addOutputPort( "Pose Pivot Point", SIGNAL( posePivotPointChanged( Point_3, QQuaternion, PoseOption::Options ) ) );
      b->addOutputPort( "Pose Tow Point", SIGNAL( poseTowPointChanged( Point_3, QQuaternion, PoseOption::Options ) ) );

      return b;
    }
};

#endif // TRACTORKINEMATIC_H
