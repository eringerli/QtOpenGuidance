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

#include "SystemModel.h"

namespace KinematicModel {
  namespace ImuMeasurementNames {
    enum ImuMeasurementNames : int {
      Roll = 0,
      Pitch,
      Yaw,
      Vroll,
      Vpitch,
      Vyaw,
      Ax,
      Ay,
      Az,
      End
    };
  }

  template<typename _T>
  using ImuMeasurementVector = Kalman::Vector<_T, 9>;

  template<typename _T, template<typename> class StateType, template<typename> class MeasurementType>
  class ImuMeasurement {
    public:
      ImuMeasurement() {
        setPresicions( 0.00174533, // 0.1°
                       0.01,
                       0.01 );

        names << QStringLiteral( "Roll" )
              << QStringLiteral( "Pitch" )
              << QStringLiteral( "Yaw" )
              << QStringLiteral( "Vroll" )
              << QStringLiteral( "Vpitch" )
              << QStringLiteral( "Vyaw" )
              << QStringLiteral( "Ax" )
              << QStringLiteral( "Ay" )
              << QStringLiteral( "Az" );
      }

      template<typename T>
      void measure( const StateType<T>& x, MeasurementType<T>& measurement ) {
        measurement.setZero();

        measurement( ImuMeasurementNames::Roll ) = normalizeAngleRadians( x( StateNames::Roll ) );
        measurement( ImuMeasurementNames::Pitch ) = normalizeAngleRadians( x( StateNames::Pitch ) );
        measurement( ImuMeasurementNames::Yaw ) = normalizeAngleRadians( x( StateNames::Yaw ) );
        measurement( ImuMeasurementNames::Vroll ) = x( StateNames::Vroll );
        measurement( ImuMeasurementNames::Vpitch ) = x( StateNames::Vpitch );
        measurement( ImuMeasurementNames::Vyaw ) = x( StateNames::Vyaw );
        measurement( ImuMeasurementNames::Ax ) = x( StateNames::Ax );
        measurement( ImuMeasurementNames::Ay ) = x( StateNames::Ay );
        measurement( ImuMeasurementNames::Az ) = x( StateNames::Az );
      }

      template<typename T>
      Kalman::Covariance <T, MeasurementType<T> > getCovariance() {
        return covariance;
      }

      void setPresicions( const double orientationAccuracy,
                          const double gyroAccuracy,
                          const double accelerometerAccuracy ) {
        double orientationAccuracyTmp = orientationAccuracy * orientationAccuracy;
        double gyroAccuracyTmp = gyroAccuracy * gyroAccuracy;
        double accelerometerAccuracyTmp = accelerometerAccuracy * accelerometerAccuracy;

        Kalman::Vector<double, 9> covVec;
        covVec << orientationAccuracyTmp, orientationAccuracyTmp, orientationAccuracyTmp,
               gyroAccuracyTmp, gyroAccuracyTmp, gyroAccuracyTmp,
               accelerometerAccuracyTmp, accelerometerAccuracyTmp, accelerometerAccuracyTmp;
        covariance = covVec.asDiagonal();

      }


      const QString getName( ImuMeasurementNames::ImuMeasurementNames state ) {
        if( state < ImuMeasurementNames::End ) {
          return names[int( state )];
        } else {
          return QString();
        }
      }

    public:
      QStringList names;

    private:
      Kalman::Covariance<_T, MeasurementType<_T> > covariance;

      template<typename T>
      T normalizeAngleRadians( T angle ) {
        while( angle > M_PI ) {
          angle -= M_PI * 2;
        }

        while( angle < -M_PI ) {
          angle += M_PI * 2;
        }

        return angle;
      }
  };


} // namespace KalmanParameters
