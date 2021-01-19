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

#include "UbxParser.h"

#include "qneblock.h"
#include "qneport.h"

#include "helpers/anglesHelper.h"

#include <QBrush>


void UBX_Parser_Helper::handle_NAV_HPPOSLLH( uint32_t iTOW, double lon, double lat, double height, double hMSL, double hAcc, double vAcc ) {
  Q_EMIT ubxNavHpPosLlhChanged( iTOW, lon, lat, height, hMSL, hAcc, vAcc );
}

void UBX_Parser_Helper::handle_NAV_RELPOSNED( uint32_t iTOW, double relPosN, double relPosE, double relPosD, double relPosLenght, double relPosHeading ) {
  Q_EMIT ubxNavRelPosNedChanged( iTOW, relPosN, relPosE, relPosD, relPosLenght, relPosHeading );
}

void UBX_Parser_Helper::handle_NAV_PVT( uint32_t iTOW, bool rtkFix, bool rtkFloat, uint8_t numSatelites, double speed, double headingOfMotion, double headingOfVehicle, double pDOP ) {
  Q_EMIT UbxNavPvtChanged( iTOW, rtkFix, rtkFloat, numSatelites, speed, headingOfMotion, headingOfVehicle, pDOP );
}

UbxParser::UbxParser()
  : BlockBase() {
  connect( &ubxParser, &UBX_Parser_Helper::ubxNavHpPosLlhChanged, this, &UbxParser::ubxNavHpPosLLH );
  connect( &ubxParser, &UBX_Parser_Helper::ubxNavRelPosNedChanged, this, &UbxParser::ubxNavRelPosNed );
  connect( &ubxParser, &UBX_Parser_Helper::UbxNavPvtChanged, this, &UbxParser::UbxNavPvt );
}

void UbxParser::setData( const QByteArray& data ) {
  for( const uint8_t c : data ) {
    ubxParser.parse( c );
  }
}

void UbxParser::ubxNavHpPosLLH( uint32_t, double lon, double lat, double height, double, double hAcc, double vAcc ) {
  Q_EMIT globalPositionChanged( Eigen::Vector3d( lon, lat, height ) );
  Q_EMIT horizontalAccuracyChanged( hAcc );
  Q_EMIT verticalAccuracyChanged( vAcc );
}

void UbxParser::ubxNavRelPosNed( uint32_t, double, double, double relPosD, double relPosLenght, double relPosHeading ) {
  Q_EMIT orientationDualAntennaChanged(
          // roll
          Eigen::AngleAxisd( ( std::asin( relPosD / relPosLenght ) - rollOffsetRad )*rollFactor, Eigen::Vector3d::UnitX() )
          *
          // heading
          Eigen::AngleAxisd( ( degreesToRadians( relPosHeading ) - headingOffsetRad )*headingFactor, Eigen::Vector3d::UnitZ() )
  );
  Q_EMIT distanceBetweenAntennasChanged( relPosLenght );
}

void UbxParser::UbxNavPvt( uint32_t, bool, bool, uint8_t numSatelites, double speed, double headingOfMotion, double headingOfVehicle, double pDOP ) {
  Q_EMIT velocityChanged( speed );
  Q_EMIT numSatelitesChanged( numSatelites );
  Q_EMIT hdopChanged( pDOP );

  Q_EMIT orientationMotionChanged( Eigen::Quaterniond( Eigen::AngleAxisd( ( degreesToRadians( headingOfMotion ) - headingOffsetRad )*headingFactor, Eigen::Vector3d::UnitZ() ) ) );

  Q_EMIT orientationMotionChanged( Eigen::Quaterniond( Eigen::AngleAxisd( ( degreesToRadians( headingOfVehicle ) - headingOffsetRad )*headingFactor, Eigen::Vector3d::UnitZ() ) ) );
}

void UbxParser::setHeadingOffset( const double headingOffset ) {
  this->headingOffsetRad = degreesToRadians( headingOffset );
}

void UbxParser::setHeadingFactor( const double headingFactor ) {
  this->headingFactor = headingFactor;
}

void UbxParser::setRollOffset( const double rollOffset ) {
  this->rollOffsetRad = degreesToRadians( rollOffset );
}

void UbxParser::setRollFactor( const double rollFactor ) {
  this->rollFactor = rollFactor;
}

QNEBlock* UbxParserFactory::createBlock( QGraphicsScene* scene, int id ) {
  auto* obj = new UbxParser();
  auto* b = createBaseBlock( scene, obj, id );

  b->addInputPort( QStringLiteral( "Data" ), QLatin1String( SLOT( setData( const QByteArray& ) ) ) );
  b->addInputPort( QStringLiteral( "Heading Offset" ), QLatin1String( SLOT( setHeadingOffset( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Heading Factor" ), QLatin1String( SLOT( setHeadingFactor( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Roll Offset" ), QLatin1String( SLOT( setRollOffset( const double ) ) ) );
  b->addInputPort( QStringLiteral( "Roll Factor" ), QLatin1String( SLOT( setRollFactor( const double ) ) ) );

  b->addOutputPort( QStringLiteral( "WGS84 Position" ), QLatin1String( SIGNAL( globalPositionChanged( const Eigen::Vector3d& ) ) ) );
  b->addOutputPort( QStringLiteral( "Velocity" ), QLatin1String( SIGNAL( velocityChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation Dual Antenna" ), QLatin1String( SIGNAL( orientationDualAntennaChanged( const Eigen::Quaterniond& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation GNS/Vehicle" ), QLatin1String( SIGNAL( orientationVehicleChanged( const Eigen::Quaterniond& ) ) ) );
  b->addOutputPort( QStringLiteral( "Orientation GNS/Motion" ), QLatin1String( SIGNAL( orientationMotionChanged( const Eigen::Quaterniond& ) ) ) );

  b->addOutputPort( QStringLiteral( "Dist Antennas" ), QLatin1String( SIGNAL( distanceBetweenAntennasChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Num Satelites" ), QLatin1String( SIGNAL( numSatelitesChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "HDOP" ), QLatin1String( SIGNAL( hdopChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Horizontal Accuracy" ), QLatin1String( SIGNAL( horizontalAccuracyChanged( const double ) ) ) );
  b->addOutputPort( QStringLiteral( "Vertical Accuracy" ), QLatin1String( SIGNAL( verticalAccuracyChanged( const double ) ) ) );

  b->setBrush( parserColor );

  return b;
}
