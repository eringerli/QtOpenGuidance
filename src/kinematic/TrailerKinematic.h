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
#include <QtMath>
#include <QtDebug>

#include "../block/BlockBase.h"

#include "../kinematic/cgalKernel.h"
#include "../kinematic/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

class TrailerKinematic : public BlockBase {
    Q_OBJECT

  public:
    explicit TrailerKinematic()
      : BlockBase() {}

  public slots:
    void setOffsetTowPointPosition( Eigen::Vector3d position ) {
      m_offsetTowPoint = Eigen::Vector3d( position.x(), position.y(), position.z() );
    }
    void setOffsetHookPointPosition( Eigen::Vector3d position ) {
      m_offsetHookPoint = Eigen::Vector3d( position.x(), position.y(), position.z() );
    }

    void setMaxJackknifeAngle( double maxAngle ) {
      m_maxJackknifeAngleRad = qDegreesToRadians( maxAngle );
    }

    void setMaxAngle( double maxAngle ) {
      m_maxAngleRad = qDegreesToRadians( maxAngle );
    }

    void setPose( const Point_3 position, const Eigen::Quaterniond rotation, const PoseOption::Options options ) {
      Eigen::Quaterniond orientation = rotation;
      Eigen::Quaterniond orientationTrailer =  Eigen::Quaterniond();

      if( options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation =  Eigen::Quaterniond();
      } else {

        orientationTrailer = Eigen::AngleAxisd( std::atan2( position.y() - m_positionPivotPoint.y(),
                                                position.x() - m_positionPivotPoint.x() ),
                                                Eigen::Vector3d::UnitZ()
                                              );

        // the angle between tractor and trailer > m_maxAngleToTowingKinematic -> reset orientation to the one from the tractor
        double angle = quaternionToEuler( orientation.inverse() * orientationTrailer ).z();

        if( std::abs( angle ) < m_maxJackknifeAngleRad ) {

          // limit the angle to m_maxAngle
          if( std::abs( angle ) > m_maxAngleRad ) {
            orientation = orientation * Eigen::AngleAxisd( m_maxAngleRad * ( angle > 0 ? 1 : -1 ), Eigen::Vector3d::UnitZ() );
          } else {
            orientation = orientationTrailer;
          }
        }
      }

      Eigen::Vector3d positionPivotPointCorrection;

      if( !options.testFlag( PoseOption::CalculateFromPivotPoint ) ) {
        positionPivotPointCorrection = orientation * -m_offsetHookPoint;
      }

      Point_3 positionPivotPoint = Point_3( position.x() + double( positionPivotPointCorrection.x() ),
                                            position.y() + double( positionPivotPointCorrection.y() ),
                                            position.z() + double( positionPivotPointCorrection.z() ) );

      if( !options.testFlag( PoseOption::CalculateLocalOffsets ) ) {
        m_positionPivotPoint = positionPivotPoint;
      }

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
    // defined in the normal way: x+ is forwards, so m_offsetTowPoint is a negative vector
    Eigen::Vector3d m_offsetHookPoint = Eigen::Vector3d( 6, 0, 0 );
    Eigen::Vector3d m_offsetTowPoint = Eigen::Vector3d( -1, 0, 0 );
    Point_3 m_positionPivotPoint = Point_3( 0, 0, 0 );

    double m_maxJackknifeAngleRad = qDegreesToRadians( double( 120 ) );
    double m_maxAngleRad = qDegreesToRadians( double( 150 ) );
};

class TrailerKinematicFactory : public BlockFactory {
    Q_OBJECT

  public:
    TrailerKinematicFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Kinematic" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new TrailerKinematic();
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "OffsetHookPoint" ), QLatin1String( SLOT( setOffsetHookPointPosition( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "OffsetTowPoint" ), QLatin1String( SLOT( setOffsetTowPointPosition( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( double ) ) ) );
      b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Hook Point" ), QLatin1String( SIGNAL( poseHookPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Pivot Point" ), QLatin1String( SIGNAL( posePivotPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );
      b->addOutputPort( QStringLiteral( "Pose Tow Point" ), QLatin1String( SIGNAL( poseTowPointChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }
};
