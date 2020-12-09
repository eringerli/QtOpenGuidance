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

#include "FixedKinematicPrimitive.h"

#include "../helpers/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

class TrailerKinematicPrimitive : public BlockBase {
    Q_OBJECT

  public:
    explicit TrailerKinematicPrimitive()
      : BlockBase() {}

  public Q_SLOTS:
    void setOffset( const Eigen::Vector3d& offset ) {
      fixedKinematic.setOffset( offset );
    }
    void setMaxJackknifeAngle( const double maxJackknifeAngle ) {
      maxJackknifeAngleRad = qDegreesToRadians( maxJackknifeAngle );
    }
    void setMaxAngle( const double maxAngle ) {
      maxAngleRad = qDegreesToRadians( maxAngle );
    }

    void setPose( const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation, const PoseOption::Options& options ) {
      if( options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation = Eigen::Quaterniond();
      } else {
        double angleRad = std::atan2( position.y() - positionCalculated.y(),
                                      position.x() - positionCalculated.x() );

        Eigen::Quaterniond orientationTmp;
        orientationTmp = Eigen::AngleAxisd( angleRad,
                                            Eigen::Vector3d::UnitZ()
                                          );

        // the angle between tractor and trailer > maxAngleToTowingKinematic -> reset orientation to the one from the tractor
        double angleDifferenceRad = quaternionToTaitBryan( rotation.inverse() * orientationTmp ).z();

        if( std::abs( angleDifferenceRad ) < maxJackknifeAngleRad ) {
          // limit the angle to maxAngle
          if( std::abs( angleDifferenceRad ) > maxAngleRad ) {
            orientation = rotation * Eigen::AngleAxisd( maxAngleRad * ( angleDifferenceRad > 0 ? 1 : -1 ), Eigen::Vector3d::UnitZ() );
          } else {
            orientation = orientationTmp;
          }
        } else {
          orientation = rotation;
        }
      }

      fixedKinematic.setPose( position, orientation, options );
      positionCalculated = fixedKinematic.positionCalculated;

      Q_EMIT poseChanged( positionCalculated, orientation, options );
    }

  Q_SIGNALS:
    void poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& );

  public:
    Eigen::Vector3d positionCalculated = Eigen::Vector3d( -1, 0, 0 );
    Eigen::Quaterniond orientation =  Eigen::Quaterniond();

    double maxJackknifeAngleRad = qDegreesToRadians( double( 120 ) );
    double maxAngleRad = qDegreesToRadians( double( 150 ) );

    FixedKinematicPrimitive fixedKinematic;
};

class TrailerKinematicPrimitiveFactory : public BlockFactory {
    Q_OBJECT

  public:
    TrailerKinematicPrimitiveFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Trailer Kinematic Primitive" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new TrailerKinematicPrimitive;
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Offset In to Out" ), QLatin1String( SLOT( setOffset( const Eigen::Vector3d& ) ) ) );
      b->addInputPort( QStringLiteral( "MaxJackknifeAngle" ), QLatin1String( SLOT( setMaxJackknifeAngle( const double ) ) ) );
      b->addInputPort( QStringLiteral( "MaxAngle" ), QLatin1String( SLOT( setMaxAngle( const double ) ) ) );
      b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( const Eigen::Vector3d&, const Eigen::Quaterniond&, const PoseOption::Options& ) ) ) );

      return b;
    }
};
