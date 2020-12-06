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

#include "../helpers/cgalHelper.h"
#include "../helpers/eigenHelper.h"
#include "../kinematic/PoseOptions.h"

class FixedKinematicPrimitive : public BlockBase {
    Q_OBJECT

  public:
    explicit FixedKinematicPrimitive()
      : BlockBase() {}

  public slots:
    void setOffset( Eigen::Vector3d offset ) {
      this->offset = -offset;
    }

    void setPose( const Point_3 position, const Eigen::Quaterniond rotation, const PoseOption::Options options ) {
      if( !options.testFlag( PoseOption::CalculateWithoutOrientation ) ) {
        orientation = rotation;
      } else {
        orientation = Eigen::Quaterniond();
      }

      Eigen::Vector3d positionCorrection = orientation * offset;

      positionCalculated = Point_3( position.x() + positionCorrection.x(),
                                    position.y() + positionCorrection.y(),
                                    position.z() + positionCorrection.z() );

      emit poseChanged( positionCalculated, orientation, options );
    }

  signals:
    void poseChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options );

  public:
    Point_3 positionCalculated = Point_3( 0, 0, 0 );
    Eigen::Quaterniond orientation = Eigen::Quaterniond();
    Eigen::Vector3d offset = Eigen::Vector3d( -1, 0, 0 );
};

class FixedKinematicPrimitiveFactory : public BlockFactory {
    Q_OBJECT

  public:
    FixedKinematicPrimitiveFactory()
      : BlockFactory() {}

    QString getNameOfFactory() override {
      return QStringLiteral( "Fixed Kinematic Primitive" );
    }

    QString getCategoryOfFactory() override {
      return QStringLiteral( "Calculations" );
    }

    virtual QNEBlock* createBlock( QGraphicsScene* scene, int id ) override {
      auto* obj = new FixedKinematicPrimitive;
      auto* b = createBaseBlock( scene, obj, id );

      b->addInputPort( QStringLiteral( "Offset" ), QLatin1String( SLOT( setOffset( Eigen::Vector3d ) ) ) );
      b->addInputPort( QStringLiteral( "Pose In" ), QLatin1String( SLOT( setPose( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      b->addOutputPort( QStringLiteral( "Pose Out" ), QLatin1String( SIGNAL( poseChanged( const Point_3, const Eigen::Quaterniond, const PoseOption::Options ) ) ) );

      return b;
    }
};
