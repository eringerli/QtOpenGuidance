// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "UbxParser.h"

#include "helpers/anglesHelper.h"

void
UBX_Parser_Helper::handle_NAV_HPPOSLLH( uint32_t iTOW, double lon, double lat, double height, double hMSL, double hAcc, double vAcc ) {
  Q_EMIT ubxNavHpPosLlhChanged( iTOW, lon, lat, height, hMSL, hAcc, vAcc );
}

void
UBX_Parser_Helper::handle_NAV_RELPOSNED(
  uint32_t iTOW, double relPosN, double relPosE, double relPosD, double relPosLenght, double relPosHeading ) {
  Q_EMIT ubxNavRelPosNedChanged( iTOW, relPosN, relPosE, relPosD, relPosLenght, relPosHeading );
}

void
UBX_Parser_Helper::handle_NAV_PVT( uint32_t iTOW,
                                   bool     rtkFix,
                                   bool     rtkFloat,
                                   uint8_t  numSatelites,
                                   double   speed,
                                   double   headingOfMotion,
                                   double   headingOfVehicle,
                                   double   pDOP ) {
  Q_EMIT UbxNavPvtChanged( iTOW, rtkFix, rtkFloat, numSatelites, speed, headingOfMotion, headingOfVehicle, pDOP );
}

UbxParser::UbxParser( const BlockBaseId idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {
  connect( &ubxParser, &UBX_Parser_Helper::ubxNavHpPosLlhChanged, this, &UbxParser::ubxNavHpPosLLH );
  connect( &ubxParser, &UBX_Parser_Helper::ubxNavRelPosNedChanged, this, &UbxParser::ubxNavRelPosNed );
  connect( &ubxParser, &UBX_Parser_Helper::UbxNavPvtChanged, this, &UbxParser::UbxNavPvt );
}

void
UbxParser::setData( const QByteArray& data ) {
  for( const uint8_t c : data ) {
    ubxParser.parse( c );
  }
}

void
UbxParser::ubxNavHpPosLLH( uint32_t, double lon, double lat, double height, double, double hAcc, double vAcc ) {
  Q_EMIT globalPositionChanged( Eigen::Vector3d( lon, lat, height ), CalculationOption::Option::None );
  Q_EMIT horizontalAccuracyChanged( hAcc, CalculationOption::Option::None );
  Q_EMIT verticalAccuracyChanged( vAcc, CalculationOption::Option::None );
}

void
UbxParser::ubxNavRelPosNed( uint32_t, double, double, double relPosD, double relPosLenght, double relPosHeading ) {
  Q_EMIT orientationDualAntennaChanged(
    // roll
    Eigen::AngleAxisd( ( std::asin( relPosD / relPosLenght ) - rollOffsetRad ) * rollFactor, Eigen::Vector3d::UnitX() ) *
      // heading
      Eigen::AngleAxisd( ( degreesToRadians( relPosHeading ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ),
    CalculationOption::Option::None );
  Q_EMIT distanceBetweenAntennasChanged( relPosLenght, CalculationOption::Option::None );
}

void
UbxParser::UbxNavPvt(
  uint32_t, bool, bool, uint8_t numSatelites, double speed, double headingOfMotion, double headingOfVehicle, double pDOP ) {
  Q_EMIT velocityChanged( speed, CalculationOption::Option::None );
  Q_EMIT numSatelitesChanged( numSatelites, CalculationOption::Option::None );
  Q_EMIT hdopChanged( pDOP, CalculationOption::Option::None );

  Q_EMIT orientationMotionChanged(
    Eigen::Quaterniond(
      Eigen::AngleAxisd( ( degreesToRadians( headingOfMotion ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ) ),
    CalculationOption::Option::None );

  Q_EMIT orientationMotionChanged(
    Eigen::Quaterniond(
      Eigen::AngleAxisd( ( degreesToRadians( headingOfVehicle ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ) ),
    CalculationOption::Option::None );
}

void
UbxParser::setHeadingOffset( const double headingOffset, CalculationOption::Options ) {
  this->headingOffsetRad = degreesToRadians( headingOffset );
}

void
UbxParser::setHeadingFactor( const double headingFactor, CalculationOption::Options ) {
  this->headingFactor = headingFactor;
}

void
UbxParser::setRollOffset( const double rollOffset, CalculationOption::Options ) {
  this->rollOffsetRad = degreesToRadians( rollOffset );
}

void
UbxParser::setRollFactor( const double rollFactor, CalculationOption::Options ) {
  this->rollFactor = rollFactor;
}

std::unique_ptr< BlockBase >
UbxParserFactory::createBlock( const BlockBaseId idHint ) {
  auto obj = createBaseBlock< UbxParser >( idHint );

  obj->addInputPort( QStringLiteral( "Data" ), obj.get(), QLatin1StringView( SLOT( setData( const QByteArray& ) ) ) );
  obj->addInputPort( QStringLiteral( "Heading Offset" ), obj.get(), QLatin1StringView( SLOT( setHeadingOffset( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Heading Factor" ), obj.get(), QLatin1StringView( SLOT( setHeadingFactor( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Roll Offset" ), obj.get(), QLatin1StringView( SLOT( setRollOffset( NUMBER_SIGNATURE ) ) ) );
  obj->addInputPort( QStringLiteral( "Roll Factor" ), obj.get(), QLatin1StringView( SLOT( setRollFactor( NUMBER_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "WGS84 Position" ), obj.get(), QLatin1StringView( SIGNAL( globalPositionChanged( VECTOR_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Velocity" ), obj.get(), QLatin1StringView( SIGNAL( velocityChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Orientation Dual Antenna" ),
                      obj.get(),
                      QLatin1StringView( SIGNAL( orientationDualAntennaChanged( ORIENTATION_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Orientation GNS/Vehicle" ),
                      obj.get(),
                      QLatin1StringView( SIGNAL( orientationVehicleChanged( ORIENTATION_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "Orientation GNS/Motion" ),
                      obj.get(),
                      QLatin1StringView( SIGNAL( orientationMotionChanged( ORIENTATION_SIGNATURE ) ) ) );

  obj->addOutputPort(
    QStringLiteral( "Dist Antennas" ), obj.get(), QLatin1StringView( SIGNAL( distanceBetweenAntennasChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Num Satelites" ), obj.get(), QLatin1StringView( SIGNAL( numSatelitesChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort( QStringLiteral( "HDOP" ), obj.get(), QLatin1StringView( SIGNAL( hdopChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Horizontal Accuracy" ), obj.get(), QLatin1StringView( SIGNAL( horizontalAccuracyChanged( NUMBER_SIGNATURE ) ) ) );
  obj->addOutputPort(
    QStringLiteral( "Vertical Accuracy" ), obj.get(), QLatin1StringView( SIGNAL( verticalAccuracyChanged( NUMBER_SIGNATURE ) ) ) );

  return obj;
}
