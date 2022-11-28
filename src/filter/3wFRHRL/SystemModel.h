// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDebug>
#include <QtMath>

#include <kalman/ExtendedKalmanFilter.hpp>

#include <iomanip>
#include <iostream>

// Formulas from here:
// yaw dynamic model: https://etd.auburn.edu/bitstream/handle/10415/819/PEARSON_PAUL_30.pdf;sequence=1
// transfer function: https://github.com/cra-ros-pkg/robot_localization/blob/melodic-devel/src/ekf.cpp#L210

namespace ThreeWheeledFRHRL {
  namespace StateNames {
    enum StateNames : int {
      X = 0,
      Y,
      Z,
      Roll,
      Pitch,
      Yaw,
      SteerAngle,
      Vx,
      Vy,
      Vz,
      Vroll,
      Vpitch,
      Vyaw,
      SteerAngleRate,
      Ax,
      Ay,
      Az,
      AlphaFront,
      AlphaRear,
      AlphaHitch,
      End
    };
  }

  template< typename _T >
  using State = Kalman::Vector< _T, StateNames::End >;

  template< template< typename > class StateType >
  class TractorImuGpsFusionModel {
  public:
    TractorImuGpsFusionModel() {
      /* clang-format off */
      names << QStringLiteral( "X" )
            << QStringLiteral( "Y" )
            << QStringLiteral( "Z" )
            << QStringLiteral( "Roll" )
            << QStringLiteral( "Pitch" )
            << QStringLiteral( "Yaw" )
            << QStringLiteral( "SteerAngle" )
            << QStringLiteral( "Vx" )
            << QStringLiteral( "Vy" )
            << QStringLiteral( "Vz" )
            << QStringLiteral( "Vroll" )
            << QStringLiteral( "Vpitch" )
            << QStringLiteral( "Vyaw" )
            << QStringLiteral( "SteerAngleRate" )
            << QStringLiteral( "Ax" )
            << QStringLiteral( "Ay" )
            << QStringLiteral( "Az" )
            << QStringLiteral( "AlphaFront" )
            << QStringLiteral( "AlphaRear" )
            << QStringLiteral( "AlphaHitch" );
      /* clang-format on */

      oldResult.setZero();
    }

    template< typename T, typename _T >
    void predict( const StateType< T >& x,
                  const _T              deltaT,
                  const _T              V,
                  const _T              deltaF,
                  const _T              a,
                  const _T              b,
                  const _T              c,
                  const _T              Caf,
                  const _T              Car,
                  const _T              Cah,
                  const _T              m,
                  const _T              Iz,
                  const _T              sigmaF,
                  const _T              sigmaR,
                  const _T              sigmaH,
                  const _T              Cx,
                  const _T              slipX,
                  StateType< T >&       prediction ) {
      constexpr bool enableDebug = false;

      // this implements formula 2.58 on page 21 with the 4WD part from formula 5.9 on page 79 of
      // MODELING AND VALIDATION OF HITCHED LOADING EFFECTS ON TRACTOR YAW DYNAMICS
      // from Paul James Pearson
      // ATTENTION: The direction of the vectors and angles in the paper are opposite of the direction used here!!!

      StateType< T > xBuffer;
      xBuffer = x;

      if( V > 0.5 ) {
        // model isn't compatible with negative velocities (driving backwards), so use the straight kinematic model
        {
          // matrix for the first term (formula 2.58 + 5.9)
          const _T                      Ftrac = Cx * slipX;
          Eigen::Matrix< double, 5, 5 > firstTerm;
          firstTerm << -Car / ( m * V ), ( b * Car ) / ( m * V ) - V, -( Ftrac + Caf ) / m, -Car / m, -Cah / m, ( b * Car ) / ( Iz * V ),
            -( b * b * Car ) / ( Iz * V ), ( -( Ftrac * a + Caf * a ) ) / Iz, ( b * Car ) / Iz, ( ( b + c ) * Cah ) / Iz, 1 / sigmaF,
            a / sigmaF, -V / sigmaF, 0, 0, 1 / sigmaR, -b / sigmaR, 0, -V / sigmaR, 0, 1 / sigmaH, -( b + c ) / sigmaH, 0, 0, -V / sigmaH;

          if( enableDebug ) {
            std::cout << "firstTerm" << std::endl << firstTerm << std::endl;
          }

          Eigen::Matrix< double, 5, 1 > secondTerm;
          secondTerm << -xBuffer( StateNames::Vy ), -xBuffer( StateNames::Vyaw ), -xBuffer( StateNames::AlphaFront ),
            -xBuffer( StateNames::AlphaRear ), -xBuffer( StateNames::AlphaHitch );

          if( enableDebug ) {
            std::cout << "secondTerm" << std::endl << secondTerm << std::endl;
          }

          Eigen::Matrix< double, 5, 1 > thirdTerm;
          thirdTerm << 0, 0, -V / sigmaF, 0, 0;

          if( enableDebug ) {
            std::cout << "thirdTerm" << std::endl << thirdTerm << std::endl;
          }

          if( enableDebug ) {
            std::cout << "deltaF" << std::endl << deltaF << std::endl;
          }

          auto result = ( firstTerm * secondTerm ) + ( thirdTerm * -deltaF );

          // Trapezoidal rule:
          // https://en.wikipedia.org/wiki/Trapezoidal_rule
          auto result2 = ( result + oldResult ) * ( deltaT / 2 );

          xBuffer( StateNames::Vy ) -= result2( 0 );
          xBuffer( StateNames::Vyaw ) -= result2( 1 );
          xBuffer( StateNames::AlphaFront ) = xBuffer( StateNames::AlphaFront ) - result2( 2 );
          xBuffer( StateNames::AlphaRear )  = xBuffer( StateNames::AlphaRear ) - result2( 3 );
          xBuffer( StateNames::AlphaHitch ) = xBuffer( StateNames::AlphaHitch ) - result2( 4 );

          oldResult = result;

          if( enableDebug ) {
            std::cout << "result" << std::endl << result << std::endl;
          }

          //          std::cout << "xBuffer" << std::endl << xBuffer << std::endl;
          if( enableDebug ) {
            for( int i = 0, end = names.count(); i < end; ++i ) {
              std::cout << std::fixed << std::setw( 12 ) << std::setprecision( 10 ) << /*std::scientific<<*/ std::showpos << xBuffer( i )
                        << " " << names[i].toStdString() << std::endl;
            }
          }

          if( enableDebug ) {
            qDebug() << "ThreeWheeledFRHRL::predict results" << xBuffer( StateNames::Vy ) << xBuffer( StateNames::Vyaw )
                     << radiansToDegrees( xBuffer( StateNames::AlphaFront ) ) << radiansToDegrees( xBuffer( StateNames::AlphaRear ) )
                     << radiansToDegrees( xBuffer( StateNames::AlphaHitch ) );
          }
        }
      } else if( V < -0.1 || V > -0.1 ) {
        xBuffer( StateNames::Vyaw )       = std::tan( deltaF ) * V / ( a + b );
        xBuffer( StateNames::Vy )         = 0;
        xBuffer( StateNames::AlphaFront ) = 0;
        xBuffer( StateNames::AlphaRear )  = 0;
        xBuffer( StateNames::AlphaHitch ) = 0;
      } else {
        xBuffer( StateNames::Vy )         = 0;
        xBuffer( StateNames::Vyaw )       = 0;
        xBuffer( StateNames::AlphaFront ) = 0;
        xBuffer( StateNames::AlphaRear )  = 0;
        xBuffer( StateNames::AlphaHitch ) = 0;
      }

      xBuffer( StateNames::Vx ) = V;

      prediction = prepareTransferMatrix( xBuffer, deltaT ) * xBuffer;

      prediction( StateNames::Yaw )        = normalizeAngleRadians( prediction( StateNames::Yaw ) );
      prediction( StateNames::Roll )       = normalizeAngleRadians( prediction( StateNames::Roll ) );
      prediction( StateNames::Pitch )      = normalizeAngleRadians( prediction( StateNames::Pitch ) );
      prediction( StateNames::AlphaFront ) = normalizeAngleRadians( prediction( StateNames::AlphaFront ) );
      prediction( StateNames::AlphaRear )  = normalizeAngleRadians( prediction( StateNames::AlphaRear ) );
      prediction( StateNames::AlphaHitch ) = normalizeAngleRadians( prediction( StateNames::AlphaHitch ) );
    }

    template< typename T, typename _T >
    void predict( const StateType< T >& x, const _T deltaT, StateType< T >& prediction ) {
      prediction = prepareTransferMatrix( x, deltaT ) * x;
    }

    template< typename T >
    Kalman::Covariance< T, StateType< T > > getCovariance() {
      return Kalman::Covariance< T, StateType< T > >::Identity();
    }

    const QString getName( StateNames::StateNames state ) {
      if( state < StateNames::End ) {
        return names[int( state )];
      } else {
        return QString();
      }
    }

  public:
    QStringList                   names;
    Eigen::Matrix< double, 5, 1 > oldResult;

  private:
    template< typename T, typename _T >
    Eigen::Matrix< T, StateNames::End, StateNames::End > prepareTransferMatrix( const StateType< T >& x, const _T deltaT ) {
      Eigen::Matrix< T, StateNames::End, StateNames::End > transferFunction_;
      transferFunction_.setIdentity();

      auto roll  = x( StateNames::Roll );
      auto pitch = x( StateNames::Pitch );
      auto yaw   = x( StateNames::Yaw );

      // We'll need these trig calculations a lot.
      auto sinPitch                = sin( pitch );
      auto cosPitch                = cos( pitch );
      auto cosPitchInverted        = 1.0 / cosPitch;
      auto sinPitchCosPithInverted = sinPitch * cosPitchInverted;

      auto sinRoll = sin( roll );
      auto cosRoll = cos( roll );

      auto sinYaw = sin( yaw );
      auto cosYaw = cos( yaw );

      // Prepare the transfer function
      transferFunction_( StateNames::X, StateNames::Vx )         = cosYaw * cosPitch * deltaT;
      transferFunction_( StateNames::X, StateNames::Vy )         = ( cosYaw * sinPitch * sinRoll - sinYaw * cosRoll ) * deltaT;
      transferFunction_( StateNames::X, StateNames::Vz )         = ( cosYaw * sinPitch * cosRoll + sinYaw * sinRoll ) * deltaT;
      transferFunction_( StateNames::X, StateNames::Ax )         = 0.5 * transferFunction_( StateNames::X, StateNames::Vx ) * deltaT;
      transferFunction_( StateNames::X, StateNames::Ay )         = 0.5 * transferFunction_( StateNames::X, StateNames::Vy ) * deltaT;
      transferFunction_( StateNames::X, StateNames::Az )         = 0.5 * transferFunction_( StateNames::X, StateNames::Vz ) * deltaT;
      transferFunction_( StateNames::Y, StateNames::Vx )         = sinYaw * cosPitch * deltaT;
      transferFunction_( StateNames::Y, StateNames::Vy )         = ( sinYaw * sinPitch * sinRoll + cosYaw * cosRoll ) * deltaT;
      transferFunction_( StateNames::Y, StateNames::Vz )         = ( sinYaw * sinPitch * cosRoll - cosYaw * sinRoll ) * deltaT;
      transferFunction_( StateNames::Y, StateNames::Ax )         = 0.5 * transferFunction_( StateNames::Y, StateNames::Vx ) * deltaT;
      transferFunction_( StateNames::Y, StateNames::Ay )         = 0.5 * transferFunction_( StateNames::Y, StateNames::Vy ) * deltaT;
      transferFunction_( StateNames::Y, StateNames::Az )         = 0.5 * transferFunction_( StateNames::Y, StateNames::Vz ) * deltaT;
      transferFunction_( StateNames::Z, StateNames::Vx )         = -sinPitch * deltaT;
      transferFunction_( StateNames::Z, StateNames::Vy )         = cosPitch * sinRoll * deltaT;
      transferFunction_( StateNames::Z, StateNames::Vz )         = cosPitch * cosRoll * deltaT;
      transferFunction_( StateNames::Z, StateNames::Ax )         = 0.5 * transferFunction_( StateNames::Z, StateNames::Vx ) * deltaT;
      transferFunction_( StateNames::Z, StateNames::Ay )         = 0.5 * transferFunction_( StateNames::Z, StateNames::Vy ) * deltaT;
      transferFunction_( StateNames::Z, StateNames::Az )         = 0.5 * transferFunction_( StateNames::Z, StateNames::Vz ) * deltaT;
      transferFunction_( StateNames::Roll, StateNames::Vroll )   = deltaT;
      transferFunction_( StateNames::Roll, StateNames::Vpitch )  = sinRoll * sinPitchCosPithInverted * deltaT;
      transferFunction_( StateNames::Roll, StateNames::Vyaw )    = cosRoll * sinPitchCosPithInverted * deltaT;
      transferFunction_( StateNames::Pitch, StateNames::Vpitch ) = cosRoll * deltaT;
      transferFunction_( StateNames::Pitch, StateNames::Vyaw )   = -sinRoll * deltaT;
      transferFunction_( StateNames::Yaw, StateNames::Vpitch )   = sinRoll * cosPitchInverted * deltaT;
      transferFunction_( StateNames::Yaw, StateNames::Vyaw )     = cosRoll * cosPitchInverted * deltaT;
      transferFunction_( StateNames::Vx, StateNames::Ax )        = deltaT;
      transferFunction_( StateNames::Vy, StateNames::Ay )        = deltaT;
      transferFunction_( StateNames::Vz, StateNames::Az )        = deltaT;

      return transferFunction_;
    }
  };
} // namespace KalmanParameters
