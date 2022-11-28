// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "SystemModel.h"

namespace KinematicModel {
  namespace PositionMeasurementNames {
    enum PositionMeasurementNames : int { X = 0, Y, Z, Vx, Vy, Vz, End };
  }

  template< typename _T >
  using PositionMeasurementVector = Kalman::Vector< _T, 6 >;

  template< typename _T, template< typename > class StateType, template< typename > class MeasurementType >
  class PositionMeasurement {
  public:
    PositionMeasurement() {
      setPresicions( 0.01, 0.02, 0.01 );

      /* clang-format off */
      names << QStringLiteral( "X" )
            << QStringLiteral( "Y" )
            << QStringLiteral( "Z" )
            << QStringLiteral( "Vx" )
            << QStringLiteral( "Vy" )
            << QStringLiteral( "Vz" );
      /* clang-format on */
    }

    template< typename T >
    void measure( const StateType< T >& x, MeasurementType< T >& measurement ) {
      measurement.setZero();

      measurement( PositionMeasurementNames::X )  = x( StateNames::X );
      measurement( PositionMeasurementNames::Y )  = x( StateNames::Y );
      measurement( PositionMeasurementNames::Z )  = x( StateNames::Z );
      measurement( PositionMeasurementNames::Vx ) = x( StateNames::Vx );
      measurement( PositionMeasurementNames::Vy ) = x( StateNames::Vy );
      measurement( PositionMeasurementNames::Vz ) = x( StateNames::Vz );
    }

    template< typename T >
    Kalman::Covariance< T, MeasurementType< T > > getCovariance() {
      return covariance;
    }

    void setPresicions( const double horizontalAccuracy, const double verticalAccuracy, const double velocityAccuracy ) {
      double horizontalAccuracyTmp = horizontalAccuracy * horizontalAccuracy;
      double velocityAccuracyTmp   = velocityAccuracy * velocityAccuracy;

      Kalman::Vector< double, 6 > covVec;
      covVec << horizontalAccuracyTmp, horizontalAccuracyTmp, verticalAccuracy * verticalAccuracy, velocityAccuracyTmp, velocityAccuracyTmp,
        velocityAccuracyTmp;
      covariance = covVec.asDiagonal();
    }

    const QString getName( PositionMeasurementNames::PositionMeasurementNames state ) {
      if( state < PositionMeasurementNames::End ) {
        return names[int( state )];
      } else {
        return QString();
      }
    }

  public:
    QStringList names;

  private:
    Kalman::Covariance< _T, MeasurementType< _T > > covariance;
  };

} // namespace KalmanParameters
