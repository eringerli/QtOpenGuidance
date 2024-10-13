// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OrientationBlock.h"

#include "kinematic/CalculationOptions.h"

#include "gui/model/OrientationBlockModel.h"

#include <QJsonObject>

void
OrientationBlock::emitConfigSignals() {
  BlockBase::emitConfigSignals();
  Q_EMIT orientationChanged( orientation, CalculationOption::Option::None );
}

void
OrientationBlock::toJSON( QJsonObject& valuesObject ) const {
  valuesObject[QStringLiteral( "X" )] = orientation.x();
  valuesObject[QStringLiteral( "Y" )] = orientation.y();
  valuesObject[QStringLiteral( "Z" )] = orientation.z();
  valuesObject[QStringLiteral( "W" )] = orientation.w();
}

void
OrientationBlock::fromJSON( const QJsonObject& valuesObject ) {
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

std::unique_ptr< BlockBase >
OrientationBlockFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< OrientationBlock >( idHint );

  obj->addInputPort( QStringLiteral( "Averager Enabled" ), obj.get(), QLatin1StringView( SLOT( setAveragerEnabled( ACTION_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Pose" ), obj.get(), QLatin1StringView( SLOT( setPose( POSE_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SLOT( setOrientation( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Orientation" ), obj.get(), QLatin1StringView( SIGNAL( orientationChanged( ORIENTATION_SIGNATURE ) ) ) );

  model->resetModel();

  return obj;
}
