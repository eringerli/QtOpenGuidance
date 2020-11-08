// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see < https : //www.gnu.org/licenses/>.

#pragma once

#include <QtMath>
#include <QDebug>

#include <kalman/ExtendedKalmanFilter.hpp>

// Formulas from here: https://github.com/cra-ros-pkg/robot_localization/blob/melodic-devel/src/ekf.cpp#L210

namespace KinematicModel {

  namespace StateNames {
    enum StateNames : int {
      X = 0,
      Y,
      Z,
      Roll,
      Pitch,
      Yaw,
      Vx,
      Vy,
      Vz,
      Vroll,
      Vpitch,
      Vyaw,
      Ax,
      Ay,
      Az,
      End
    };
  }

//  template<typename T>
//  class State : public Kalman::Vector<T, 15> {
//    public:
//      KALMAN_VECTOR( State, T, 15 )
//  };

  template<typename _T>
  using State = Kalman::Vector<_T, 15>;

  template<template<typename> class StateType>
  class TractorImuGpsFusionModel {
    public:
      TractorImuGpsFusionModel() {
        names << QStringLiteral( "X" )
              << QStringLiteral( "Y" )
              << QStringLiteral( "Z" )
              << QStringLiteral( "Roll" )
              << QStringLiteral( "Pitch" )
              << QStringLiteral( "Yaw" )
              << QStringLiteral( "Vx" )
              << QStringLiteral( "Vy" )
              << QStringLiteral( "Vz" )
              << QStringLiteral( "Vroll" )
              << QStringLiteral( "Vpitch" )
              << QStringLiteral( "Vyaw" )
              << QStringLiteral( "Ax" )
              << QStringLiteral( "Ay" )
              << QStringLiteral( "Az" );
      }

      template<typename T, typename _T>
      void predict( const StateType<T>& x, const _T deltaT,
                    const _T velocity, const _T steerAngle, const _T wheelBase,
                    StateType<T>& prediction ) {

        qDebug() << "TractorImuGpsFusionModel::predict 1" << deltaT << velocity << steerAngle << wheelBase;

        StateType<T> xBuffer;
        xBuffer = x;

        xBuffer( StateNames::Ax ) = ( velocity - x( StateNames::Vx ) ) / deltaT;
        xBuffer( StateNames::Vyaw ) = std::tan( qDegreesToRadians( steerAngle ) ) * velocity / wheelBase;

        qDebug() << "TractorImuGpsFusionModel::predict 2" << xBuffer( StateNames::Ax ) << xBuffer( StateNames::Vyaw );

        // apply the transformation
        prediction = prepareTransferMatrix( xBuffer, deltaT ) * xBuffer;
      }

      template<typename T, typename _T>
      void predict( const StateType<T>& x,
                    const _T deltaT,
                    StateType<T>& prediction ) {

        prediction = prepareTransferMatrix( x, deltaT ) * x;
      }

      template<typename T>
      Kalman::Covariance <T, StateType<T> > getCovariance() {
        return Kalman::Covariance <T, StateType<T> >::Identity();
      }

      const QString getName( StateNames::StateNames state ) {
        if( state < StateNames::End ) {
          return names[int( state )];
        } else {
          return QString();
        }
      }

    public:
      QStringList names;

    private:

      template<typename T, typename _T>
      Eigen::Matrix<T, 15, 15> prepareTransferMatrix( const StateType<T>& x, const _T deltaT ) {
        Eigen::Matrix<T, 15, 15> transferFunction_;
        transferFunction_.setIdentity();

        auto roll = x( StateNames::Roll );
        auto pitch = x( StateNames::Pitch );
        auto yaw = x( StateNames::Yaw );

        // We'll need these trig calculations a lot.
        auto sinPitch = sin( pitch );
        auto cosPitch = cos( pitch );
        auto cosPitchInverted = 1.0 / cosPitch;
        auto sinPitchCosPithInverted = sinPitch * cosPitchInverted;

        auto sinRoll = sin( roll );
        auto cosRoll = cos( roll );

        auto sinYaw = sin( yaw );
        auto cosYaw = cos( yaw );

        // Prepare the transfer function
        transferFunction_( StateNames::X, StateNames::Vx ) = cosYaw * cosPitch * deltaT;
        transferFunction_( StateNames::X, StateNames::Vy ) = ( cosYaw * sinPitch * sinRoll - sinYaw * cosRoll ) * deltaT;
        transferFunction_( StateNames::X, StateNames::Vz ) = ( cosYaw * sinPitch * cosRoll + sinYaw * sinRoll ) * deltaT;
        transferFunction_( StateNames::X, StateNames::Ax ) = 0.5 * transferFunction_( StateNames::X, StateNames::Vx ) * deltaT;
        transferFunction_( StateNames::X, StateNames::Ay ) = 0.5 * transferFunction_( StateNames::X, StateNames::Vy ) * deltaT;
        transferFunction_( StateNames::X, StateNames::Az ) = 0.5 * transferFunction_( StateNames::X, StateNames::Vz ) * deltaT;
        transferFunction_( StateNames::Y, StateNames::Vx ) = sinYaw * cosPitch * deltaT;
        transferFunction_( StateNames::Y, StateNames::Vy ) = ( sinYaw * sinPitch * sinRoll + cosYaw * cosRoll ) * deltaT;
        transferFunction_( StateNames::Y, StateNames::Vz ) = ( sinYaw * sinPitch * cosRoll - cosYaw * sinRoll ) * deltaT;
        transferFunction_( StateNames::Y, StateNames::Ax ) = 0.5 * transferFunction_( StateNames::Y, StateNames::Vx ) * deltaT;
        transferFunction_( StateNames::Y, StateNames::Ay ) = 0.5 * transferFunction_( StateNames::Y, StateNames::Vy ) * deltaT;
        transferFunction_( StateNames::Y, StateNames::Az ) = 0.5 * transferFunction_( StateNames::Y, StateNames::Vz ) * deltaT;
        transferFunction_( StateNames::Z, StateNames::Vx ) = -sinPitch * deltaT;
        transferFunction_( StateNames::Z, StateNames::Vy ) = cosPitch * sinRoll * deltaT;
        transferFunction_( StateNames::Z, StateNames::Vz ) = cosPitch * cosRoll * deltaT;
        transferFunction_( StateNames::Z, StateNames::Ax ) = 0.5 * transferFunction_( StateNames::Z, StateNames::Vx ) * deltaT;
        transferFunction_( StateNames::Z, StateNames::Ay ) = 0.5 * transferFunction_( StateNames::Z, StateNames::Vy ) * deltaT;
        transferFunction_( StateNames::Z, StateNames::Az ) = 0.5 * transferFunction_( StateNames::Z, StateNames::Vz ) * deltaT;
        transferFunction_( StateNames::Roll, StateNames::Vroll ) = deltaT;
        transferFunction_( StateNames::Roll, StateNames::Vpitch ) = sinRoll * sinPitchCosPithInverted * deltaT;
        transferFunction_( StateNames::Roll, StateNames::Vyaw ) = cosRoll * sinPitchCosPithInverted * deltaT;
        transferFunction_( StateNames::Pitch, StateNames::Vpitch ) = cosRoll * deltaT;
        transferFunction_( StateNames::Pitch, StateNames::Vyaw ) = -sinRoll * deltaT;
        transferFunction_( StateNames::Yaw, StateNames::Vpitch ) = sinRoll * cosPitchInverted * deltaT;
        transferFunction_( StateNames::Yaw, StateNames::Vyaw ) = cosRoll * cosPitchInverted * deltaT;
        transferFunction_( StateNames::Vx, StateNames::Ax ) = deltaT;
        transferFunction_( StateNames::Vy, StateNames::Ay ) = deltaT;
        transferFunction_( StateNames::Vz, StateNames::Az ) = deltaT;

        return transferFunction_;
      }
  };
} // namespace KalmanParameters
