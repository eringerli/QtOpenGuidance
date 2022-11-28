// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#include "UbxParser.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/anglesHelper.h"

#include <QBrush>

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

UbxParser::UbxParser() : BlockBase() {
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
  Q_EMIT globalPositionChanged( Eigen::Vector3d( lon, lat, height ) );
  Q_EMIT horizontalAccuracyChanged( hAcc, CalculationOption::Option::None );
  Q_EMIT verticalAccuracyChanged( vAcc, CalculationOption::Option::None );
}

void
UbxParser::ubxNavRelPosNed( uint32_t, double, double, double relPosD, double relPosLenght, double relPosHeading ) {
  Q_EMIT orientationDualAntennaChanged(
    // roll
    Eigen::AngleAxisd( ( std::asin( relPosD / relPosLenght ) - rollOffsetRad ) * rollFactor, Eigen::Vector3d::UnitX() ) *
    // heading
    Eigen::AngleAxisd( ( degreesToRadians( relPosHeading ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ) );
  Q_EMIT distanceBetweenAntennasChanged( relPosLenght, CalculationOption::Option::None );
}

void
UbxParser::UbxNavPvt(
  uint32_t, bool, bool, uint8_t numSatelites, double speed, double headingOfMotion, double headingOfVehicle, double pDOP ) {
  Q_EMIT velocityChanged( speed, CalculationOption::Option::None );
  Q_EMIT numSatelitesChanged( numSatelites, CalculationOption::Option::None );
  Q_EMIT hdopChanged( pDOP, CalculationOption::Option::None );

  Q_EMIT orientationMotionChanged( Eigen::Quaterniond(
    Eigen::AngleAxisd( ( degreesToRadians( headingOfMotion ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ) ) );

  Q_EMIT orientationMotionChanged( Eigen::Quaterniond(
    Eigen::AngleAxisd( ( degreesToRadians( headingOfVehicle ) - headingOffsetRad ) * headingFactor, Eigen::Vector3d::UnitZ() ) ) );
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

QNEBlock*
UbxParserFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new UbxParser();
  auto* b   = createBaseBlock( scene, obj, id );
  obj->moveToThread( thread );

  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
  b->addInputPort( QStringLiteral( "Heading Offset" ), QLatin1String( SLOT( setHeadingOffset( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Heading Factor" ), QLatin1String( SLOT( setHeadingFactor( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Roll Offset" ), QLatin1String( SLOT( setRollOffset( NUMBER_SIGNATURE ) ) ) );
  b->addInputPort( QStringLiteral( "Roll Factor" ), QLatin1String( SLOT( setRollFactor( NUMBER_SIGNATURE ) ) ) );

  b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const Eigen::Vector3d& ) ) ) );
  b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation Dual Antenna" ),
                    QLatin1String( SIGNAL( orientationDualAntennaChanged( const Eigen::Quaterniond& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation GNS/Vehicle" ),
                    QLatin1String( SIGNAL( orientationVehicleChanged( const Eigen::Quaterniond& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation GNS/Motion" ),
                    QLatin1String( SIGNAL( orientationMotionChanged( const Eigen::Quaterniond& ) ) ) );

  b->addOutputPort( QStringLiteral( "Dist Antennas" ), QLatin1String( SIGNAL( distanceBetweenAntennasChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Num Satelites" ), QLatin1String( SIGNAL( numSatelitesChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "HDOP" ), QLatin1String( SIGNAL( hdopChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Horizontal Accuracy" ), QLatin1String( SIGNAL( horizontalAccuracyChanged( NUMBER_SIGNATURE ) ) ) );
  b->addOutputPort( QStringLiteral( "Vertical Accuracy" ), QLatin1String( SIGNAL( verticalAccuracyChanged( NUMBER_SIGNATURE ) ) ) );

  b->setBrush( parserColor );

  return b;
}
