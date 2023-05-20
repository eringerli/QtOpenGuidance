// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OrientationBlock.h"

#include "kinematic/CalculationOptions.h"
#include "qneblock.h"
#include "qneport.h"

#include "gui/model/OrientationBlockModel.h"

#include <QBrush>
#include <QJsonObject>

void
OrientationBlock::emitConfigSignals() {
  Q_EMIT orientationChanged( orientation, CalculationOption::Option::None );
}

QJsonObject
OrientationBlock::toJSON() {
  QJsonObject valuesObject;
  valuesObject[QStringLiteral( "X" )] = orientation.x();
  valuesObject[QStringLiteral( "Y" )] = orientation.y();
  valuesObject[QStringLiteral( "Z" )] = orientation.z();
  valuesObject[QStringLiteral( "W" )] = orientation.w();
  return valuesObject;
}

void
OrientationBlock::fromJSON( QJsonObject& valuesObject ) {
  if( valuesObject[QStringLiteral( "X" )].isDouble() ) {
    orientation.x() = valuesObject[QStringLiteral( "X" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Y" )].isDouble() ) {
    orientation.y() = valuesObject[QStringLiteral( "Y" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "Z" )].isDouble() ) {
    orientation.z() = valuesObject[QStringLiteral( "Z" )].toDouble();
  }

  if( valuesObject[QStringLiteral( "W" )].isDouble() ) {
    orientation.w() = valuesObject[QStringLiteral( "W" )].toDouble();
  }
}

void
OrientationBlock::setAveragerEnabled( const bool enabled ) {
  averagerEnabledOld = averagerEnabled;
  averagerEnabled    = enabled;

  if( averagerEnabledOld != averagerEnabled ) {
    if( averagerEnabled == true ) {
      A.setZero();
      numMeasurements = 0;
      positionStart   = position;
    } else {
      A = ( 1.0 / numMeasurements ) * A;

      Eigen::SelfAdjointEigenSolver< Eigen::MatrixXd > eig( A );

      Eigen::Vector4d qavg = eig.eigenvectors().col( 3 );

      orientation = Eigen::Quaterniond( qavg );

      if( position != positionStart ) {
        auto   taitBryan = quaternionToTaitBryan( orientation );
        double heading   = std::atan2( position.y() - positionStart.y(), position.x() - positionStart.x() );
        orientation      = taitBryanToQuaternion( heading, getPitch( taitBryan ), getRoll( taitBryan ) );
      }

      Q_EMIT orientationChanged( orientation, CalculationOption::Option::None );
    }
  }
}

void
OrientationBlock::setOrientation( const Eigen::Quaterniond& orientation, CalculationOption::Options ) {
  if( averagerEnabled ) {
    ++numMeasurements;
    Eigen::Vector4d q( orientation.x(), orientation.y(), orientation.z(), orientation.w() );

    if( q[0] < 0 ) {
      q = -q;
    }

    A = q * q.adjoint() + A;
  }
}

void
OrientationBlock::setPose( const Eigen::Vector3d&           position,
                           const Eigen::Quaterniond&        orientation,
                           const CalculationOption::Options options ) {
  if( !options.testFlag( CalculationOption::CalculateLocalOffsets ) ) {
    this->position = position;

    if( averagerEnabled ) {
      ++numMeasurements;
      Eigen::Vector4d q( orientation.x(), orientation.y(), orientation.z(), orientation.w() );

      if( q[0] < 0 ) {
        q = -q;
      }

      A = q * q.adjoint() + A;
    }
  }
}

QNEBlock*
OrientationBlockFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new OrientationBlock();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Averager Enabled" ), QLatin1String( SLOT( setAveragerEnabled( ACTION_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Pose" ), QLatin1String( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Orientation" ), QLatin1String( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Orientation" ), QLatin1String( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  b->setBrush( valueColor );

  model->resetModel();

  return b;
}
