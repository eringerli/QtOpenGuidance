// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AckermannSteering.h"

#include "qneblock.h"
#include "qneport.h"

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

QNEBlock*
AckermannSteeringFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new AckermannSteering();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Length Wheelbase" ), QLatin1String( SLOT( setWheelbase( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Track Width" ), QLatin1String( SLOT( setTrackWidth( NUMBER_SIGNATURE ) ) ) );

  b->addInputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SLOT( setSteeringAngle( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SLOT( setSteeringAngleLeft( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SLOT( setSteeringAngleRight( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "Steering Angle" ), QLatin1String( SIGNAL( steeringAngleChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Steering Angle Left" ), QLatin1String( SIGNAL( steeringAngleChangedLeft( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Steering Angle Right" ), QLatin1String( SIGNAL( steeringAngleChangedRight( NUMBER_SIGNATURE ) ) ) );

  return b;
}
