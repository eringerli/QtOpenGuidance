// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AckermannSteering.h"

#include "helpers/anglesHelper.h"

void
AckermannSteering::setWheelbase( double wheelbase, const CalculationOption::Options ) {
  m_wheelbase  = wheelbase;
  m_correction = m_trackWidth / ( 2.0f * m_wheelbase );
}

void
AckermannSteering::setTrackWidth( double trackWidth, const CalculationOption::Options ) {
  m_trackWidth = trackWidth;
  m_correction = m_trackWidth / ( 2.0f * m_wheelbase );
}

void
AckermannSteering::setSteeringAngle( double steerAngle, const CalculationOption::Options ) {
  // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 ->
  // filter it out
  if( qFuzzyIsNull( steerAngle ) ) {
    Q_EMIT steeringAngleChanged( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedLeft( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedRight( 0, CalculationOption::Option::None );
  } else {
    Q_EMIT steeringAngleChanged( steerAngle, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedLeft(
      radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) - m_correction ) ) ),
      CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedRight(
      radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) + m_correction ) ) ),
      CalculationOption::Option::None );
  }
}

void
AckermannSteering::setSteeringAngleLeft( double steerAngle, const CalculationOption::Options ) {
  // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 ->
  // filter it out
  if( qFuzzyIsNull( steerAngle ) ) {
    Q_EMIT steeringAngleChanged( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedLeft( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedRight( 0, CalculationOption::Option::None );
  } else {
    Q_EMIT steeringAngleChangedLeft( steerAngle, CalculationOption::Option::None );
    Q_EMIT steeringAngleChanged( radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) + m_correction ) ) ),
                                 CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedRight(
      radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) + ( 2 * m_correction ) ) ) ),
      CalculationOption::Option::None );
  }
}

void
AckermannSteering::setSteeringAngleRight( double steerAngle, const CalculationOption::Options ) {
  // as the cotangens (1/tan()) is used, there is no valid result for steerangle=0 ->
  // filter it out
  if( qFuzzyIsNull( steerAngle ) ) {
    Q_EMIT steeringAngleChanged( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedLeft( 0, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedRight( 0, CalculationOption::Option::None );
  } else {
    Q_EMIT steeringAngleChangedRight( steerAngle, CalculationOption::Option::None );
    Q_EMIT steeringAngleChangedLeft(
      radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) - ( 2 * m_correction ) ) ) ),
      CalculationOption::Option::None );
    Q_EMIT steeringAngleChanged( radiansToDegrees( std::atan( 1 / ( ( 1 / std::tan( degreesToRadians( steerAngle ) ) ) - m_correction ) ) ),
                                 CalculationOption::Option::None );
  }
}

std::unique_ptr< BlockBase >
AckermannSteeringFactory::createBlock( int idHint ) {
  auto obj = createBaseBlock< AckermannSteering >( idHint );

  obj->addInputPort( QStringLiteral( "Length Wheelbase" ), obj.get(), QLatin1StringView( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Track Width" ), obj.get(), QLatin1StringView( SLOT( setTrackWidth( NUMBER_SIGNATURE ) ) ) );

  obj->addInputPort( QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Steering Angle Left" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngleLeft( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort(
    QStringLiteral( "Steering Angle Right" ), obj.get(), QLatin1StringView( SLOT( setSteeringAngleRight( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Steering Angle" ), obj.get(), QLatin1StringView( SIGNAL( steeringAngleChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Steering Angle Left" ), obj.get(), QLatin1StringView( SIGNAL( steeringAngleChangedLeft( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Steering Angle Right" ), obj.get(), QLatin1StringView( SIGNAL( steeringAngleChangedRight( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
