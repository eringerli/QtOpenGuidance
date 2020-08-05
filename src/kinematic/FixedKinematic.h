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

#include <QObject>

#include <QTime>
#include <QEvent>
#include <QBasicTimer>
#include <QVector3D>

#include <QtGlobal>
#include <QtDebug>

#include "../block/BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

class FixedKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematic()
      : BlockBase(),
        m_offsetHookPoint( Eigen::Vector3d( 0, 0, 0 ) ), m_offsetTowPoint( Eigen::Vector3d( -1, 0, 0 ) ) {}

  public slots:
    void setOffsetTowPointPosition( Eigen::Vector3d position ) {
      m_offsetTowPoint = position;
    }
    void setOffsetHookPointPosition( Eigen::Vector3d position ) {
      m_offsetHookPoint = position;
    }

    void setPose( const Point_3 position, const Eigen::Quaterniond rotation, const PoseOption::Options options ) {
      Eigen::Quaterniond orientation = Eigen::Quaterniond();

      if( !options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation = rotation;
      }

      Eigen::Vector3d positionPivotPointCorrection;

      if( !options.testFlag( PoseOption::CalculateFromPivotPoint ) ) {
        positionPivotPointCorrection = orientation * -m_offsetHookPoint;
      }

      Point_3 positionPivotPoint = Point_3( position.x() + double( positionPivotPointCorrection.x() ),
                                            position.y() + double( positionPivotPointCorrection.y() ),
                                            position.z() + double( positionPivotPointCorrection.z() ) );

      Eigen::Vector3d positionTowPointCorrection = orientation * m_offsetTowPoint;
      Point_3 positionTowPoint = Point_3( positionPivotPoint.x() + double( positionTowPointCorrection.x() ),
                                          positionPivotPoint.y() + double( positionTowPointCorrection.y() ),
                                          positionPivotPoint.z() + double( positionTowPointCorrection.z() ) );

      if( options.testFlag( PoseOption::CalculateFromPivotPoint ) ) {
        PoseOption::Options flags = options;
        flags.setFlag( PoseOption::CalculateFromPivotPoint, false );
        emit poseHookPointChanged( position, orientation, flags );
        emit posePivotPointChanged( positionPivotPoint, orientation, flags );
        emit poseTowPointChanged( positionTowPoint, orientation, flags );
      } else {
        emit poseHookPointChanged( position, orientation, options );
        emit posePivotPointChanged( positionPivotPoint, orientation, options );
        emit poseTowPointChanged( positionTowPoint, orientation, options );
      }
    }

  signals:
    void poseHookPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );
    void posePivotPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );
    void poseTowPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );

  private:
    // defined in the normal way: x+ is forwards, so m_offsetPivotPoint is a negative vector
    Eigen::Vector3d m_offsetHookPoint;
    Eigen::Vector3d m_offsetTowPoint;
};

class FixedKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new FixedKinematic;
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "OffsetHookPoint" ), QLatin1String( SLOT( setOffsetHookPointPosition( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "OffsetTowPoint" ), QLatin1String( SLOT( setOffsetTowPointPosition( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }
};
